#include "vfs/mmap-vfs.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>

#include "format-readers/peg.hpp"
#include "format-readers/validation-error.hpp"
#include "format-readers/vpp.hpp"
#include "vfs/vfs-entry.hpp"

bool MmapVfs::add_root_archive(const std::string& path)
{
    const auto& fs_path = std::filesystem::path(path);

    // VPP must exist
    if (!std::filesystem::exists(fs_path)) {
        spdlog::error("VFS: path '{}' does not exist", path);
        return false;
    }

    // Must not be a real directory
    if (std::filesystem::is_directory(fs_path)) {
        spdlog::error("VFS: cannot use this function to add a real directory '{}'", path);
        return false;
    }

    // Extension must be .VPP
    if (fs_path.extension().string() != ".vpp" && fs_path.extension().string() != ".VPP") {
        spdlog::error("VFS: can only use this function to add VPP archives, not '{}'", path);
        return false;
    }

    // Return it if we already added it
    const std::string absolute_path = std::filesystem::absolute(fs_path).generic_string();

    if (loaded_vpps_.count(absolute_path)) {
        return loaded_vpps_[absolute_path].entry;
    }

    VppFile vpp;

    VfsEntry root_entry;
    root_entry.size = std::filesystem::file_size(fs_path);
    root_entry.index = -1;
    root_entry.offset_in_parent = -1;
    root_entry.offset_in_root = -1;
    root_entry.name = fs_path.filename().string();
    root_entry.absolute_path = absolute_path;
    root_entry.relative_path = path;
    root_entry.extension = ".vpp";
    root_entry.parent = nullptr;
    root_entry.root = &root_entry;

    if (!map_file(vpp.mmap, fs_path))
        return false;

    vpp.info = ace3x::vpp::read_info(reinterpret_cast<const unsigned char*>(vpp.mmap.data()), root_entry.name);

    if (vpp.info.compressed) {
        if (!remap_as_decompressed_vpp(vpp.mmap, vpp.info)) {
            spdlog::error("VFS: failed to decompress file");
            return false;
        }
    }

    vpp.entry = add_entry(root_entry);

    for (const auto& vpp_entry : ace3x::vpp::read_entries(vpp.info, vpp.mmap.mapped_length())) {
        VfsEntry entry;
        entry.index = vpp_entry.index;
        entry.size = vpp_entry.size;
        entry.offset_in_parent = vpp_entry.offset;
        entry.offset_in_root = vpp_entry.offset;
        entry.name = vpp_entry.filename;
        entry.absolute_path = absolute_path + '/' + entry.name;
        entry.relative_path = vpp.entry->relative_path + '/' + entry.name;
        entry.parent = vpp.entry;
        entry.root = vpp.entry;
        entry.data = reinterpret_cast<const unsigned char*>(vpp.info.data) + entry.offset_in_parent;

        entry.extension = std::filesystem::path(entry.name).extension().string();
        std::transform(entry.extension.begin(), entry.extension.end(), entry.extension.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        VfsEntry* entry_ = add_entry(entry);
        vpp.entry->entries.push_back(entry_);

        if (entry.extension == ".peg") {
            for (const auto& peg_entry : ace3x::peg::read_entries(entry.data, entry.name)) {
                VfsEntry image;
                image.index = peg_entry.index;
                image.size = peg_entry.size;
                image.offset_in_parent = peg_entry.offset;
                image.offset_in_root = entry_->offset_in_root + peg_entry.offset;
                image.name = peg_entry.filename;
                image.absolute_path = entry.absolute_path + '/' + peg_entry.filename;
                image.relative_path = entry.relative_path + '/' + peg_entry.filename;
                image.parent = entry_;
                image.root = vpp.entry;
                image.data = reinterpret_cast<const unsigned char*>(vpp.info.data) + image.offset_in_parent;

                image.extension = std::filesystem::path(image.name).extension().string();
                std::transform(image.extension.begin(), image.extension.end(), image.extension.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });

                entry_->entries.push_back(add_entry(image));
            }
        }
    }

    loaded_vpps_[absolute_path] = std::move(vpp);

    return true;
}

bool MmapVfs::map_file(mio::mmap_source& mmap, const std::filesystem::path& path)
{
    std::error_code error;
    mmap.map(path.string(), error);

    if (error) {
        spdlog::error("VFS: Failed to map '{}': {}", path.generic_string(), error.message());
        return false;
    }

    return true;
}

bool MmapVfs::remap_as_decompressed_vpp(mio::mmap_source& mmap, ace3x::vpp::VppInfo& info)
{
    auto decomp_path {fmt::format("./temp/{}.decompressed", info.filename)};
    decomp_path = std::filesystem::absolute(decomp_path).generic_string();

    if (!std::filesystem::exists(decomp_path)) {
        spdlog::info("VFS: '{}': Decompressing from offset 0x{:04x}, size {} -> {}", info.filename, info.data_offset, info.header.compressedDataSize, info.header.uncompressedDataSize);

        if (!std::filesystem::exists("./temp")) {
            if (!std::filesystem::create_directory("./temp")) {
                spdlog::error("VFS: Failed to create './temp' directory for decompressed VPP's");
                return false;
            }
            else {
                spdlog::info("VFS: Created directory '{}' for decompressed VPP's", std::filesystem::absolute("./temp").string());
            }
        }

        std::vector<unsigned char> header_data(info.data_offset);
        std::memcpy(header_data.data(), info.data, header_data.size());
        auto decomp_data = ace3x::vpp::decompress(info.data + info.data_offset, info.header.compressedDataSize, info.header.uncompressedDataSize);

        FILE* decomp_file {fopen(decomp_path.c_str(), "w+b")};
        if (!decomp_file) {
            throw std::runtime_error(fmt::format("VFS: Failed to create decompressed file for VPP '{}'", info.filename));
        }
        fwrite(header_data.data(), header_data.size(), 1, decomp_file);
        fwrite(decomp_data.data(), decomp_data.size(), 1, decomp_file);
        fclose(decomp_file);
    }
    else {
        spdlog::info("VFS: Found file '{}'", decomp_path);
    }

    std::error_code error;
    mmap.map(decomp_path, error);

    if (error) {
        spdlog::error("VFS: Failed to map '{}'", decomp_path);
        return false;
    }

    info.data = reinterpret_cast<const unsigned char*>(mmap.data());

    return true;
}

VfsEntry* MmapVfs::add_entry(const VfsEntry& entry)
{
    if (VfsEntry* exists = get_entry(entry.absolute_path); exists) {
        spdlog::warn("VFS: Entry '{}' already exists", entry.absolute_path);
        return exists;
    }

    return &entries_.emplace_back(entry);
}

VfsEntry* MmapVfs::get_entry(const std::string& absolute_path)
{
    for (auto& entry : entries_) {
        if (entry.absolute_path == absolute_path) {
            return &entry;
        }
    }

    return nullptr;
}
