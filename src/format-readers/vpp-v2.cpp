/* SPDX-License-Identifier: GPLv3-or-later */

#include "format-readers/vpp-v2.hpp"

#include <spdlog/spdlog.h>
#include <zlib.h>

#include <filesystem>
#include <iostream>

#include "file-info.hpp"
#include "format-readers/validation-error.hpp"
#include "format-readers/vpp-common.hpp"
#include "formats/vpp-v2.hpp"

std::vector<std::uint8_t> decompress(const std::uint8_t *const data, std::uint32_t compressedSize, std::uint32_t uncompressedSize);

void VppV2::read(const FileInfo &info)
{
    VppV2Header header;

    std::memcpy(&header, info.mmap.data(), sizeof(VppV2Header));

    /* Sanity checks */
    if (header.signature != 0x51890ACE) {
        throw ValidationError("signature mismatch");
    }

    if ((header.fileCount <= 0) || (header.fileCount > 5000)) {
        throw ValidationError("bad file count " + std::to_string(header.fileCount));
    }

    if (header.version != 2) {
        throw ValidationError("bad version");
    }

    /* Read file list */
    std::vector<VppV2DirectoryEntry> dirEntries(header.fileCount);
    std::memcpy(
        dirEntries.data(),
        &info.mmap.data()[vpp_common::kChunkSize],
        header.fileCount * sizeof(VppV2DirectoryEntry));

    /* Read filenames */
    const std::uint32_t filenamesStart = vpp_common::align_to_chunk(vpp_common::kChunkSize + header.directorySize);
    std::vector<std::string> filenames;
    {
        std::ptrdiff_t offset = 0;
        do {
            filenames.push_back(std::string(reinterpret_cast<const char *>(&info.mmap.data()[filenamesStart] + offset)));
            offset += filenames.back().size() + 1;
        } while (offset < header.filenamesSize);
    }

    /* Map the file that we already decompressed or make it then map it */
    compressed_ = header.compressedDataSize != 0xFFFFFFFF;

    const mio::shared_mmap_source *mmap {nullptr};
    std::string vpp_path {info.absolute_path};

    if (compressed_) {
        const auto decompressed_filename {fmt::format("./temp/{}.decompressed", info.file_name)};
        vpp_path = std::filesystem::absolute(decompressed_filename).string();

        if (!std::filesystem::exists(decompressed_filename)) {
            if (!std::filesystem::exists("./temp")) {
                if (!std::filesystem::create_directory("./temp")) {
                    spdlog::error("Failed to create './temp' directory for decompressed VPP's");
                }
                else {
                    spdlog::info("Created directory '{}' for decompressed VPP's", std::filesystem::absolute("./path").string());
                }
            }

            auto decompressed_data = decompress(reinterpret_cast<const std::uint8_t *>(info.mmap.data()), header.compressedDataSize, header.uncompressedDataSize);
            FILE *decomp_out {fopen(decompressed_filename.c_str(), "w+")};
            if (!decomp_out) {
                throw std::runtime_error("Failed to create decompressed file for VPP " + info.file_name);
            }
            fwrite(decompressed_data.data(), decompressed_data.size(), 1, decomp_out);
            fclose(decomp_out);
        }

        std::error_code error;
        decompressed_mmap_.map(decompressed_filename, error);

        if (error) {
            spdlog::error("Failed to map '{}'", decompressed_filename);
            throw std::runtime_error("Failed to map decompressed VPP");
        }

        mmap = &decompressed_mmap_;
    }
    else {
        mmap = &info.mmap;
    }

    std::uint32_t offset = compressed_ ? 0 : vpp_common::align_to_chunk(filenamesStart + header.filenamesSize);

    for (std::uint16_t i = 0; i < header.fileCount; i++) {
        const auto filename = filenames[i];
        const auto index = i;
        const auto size = dirEntries[i].uncompressedSize;

        std::uint32_t vppOffset = compressed_ ? dirEntries[i].rtOffset : offset;
        if (!compressed_) {
            offset = vpp_common::align_to_chunk(offset + dirEntries[i].uncompressedSize);
        }

        if (size == 0) {
            spdlog::warn("'{}/{}' size is 0, skipping", info.file_name, filename);
            continue;
        }

        if (static_cast<int64_t>(vppOffset) + size >= mmap->mapped_length()) {
            spdlog::warn("'{}/{}' exceeds data size ({}), skipping", info.file_name, filename, mmap->mapped_length());
            continue;
        }

        FileInfo child;
        child.index_in_parent = index;
        child.file_name = filename;
        child.absolute_path = info.absolute_path + '/' + child.file_name;
        child.extension = std::filesystem::path(filename).extension().string();
        child.base_file_absolute_path = vpp_path;
        child.offset = vppOffset;

        std::error_code error;
        child.mmap = mio::make_mmap_source(vpp_path, vppOffset, size, error);

        if (error) {
            spdlog::error("Failed to map '{}/{}': {}", info.file_name, filename, error.message());
            continue;
        }

        entries_.push_back(child);
    }
}

const std::vector<FileInfo> &VppV2::get_entries() const
{
    return entries_;
}

bool VppV2::is_compressed() const
{
    return compressed_;
}

std::vector<std::uint8_t> decompress(const std::uint8_t *const data, std::uint32_t compressedSize, std::uint32_t uncompressedSize)
{
    std::vector<std::uint8_t> buffer(uncompressedSize);

    z_stream stream;
    stream.next_in = const_cast<std::uint8_t *>(data);
    stream.avail_in = compressedSize;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_out = buffer.data();
    stream.avail_out = uncompressedSize;

    int ret = Z_OK;

    ret = inflateInit(&stream);

    switch (ret) {
        case Z_OK: {
            break;
        }
        case Z_MEM_ERROR: {
            spdlog::warn("inflateInit: Not enough memory.");
            break;
        }
        case Z_VERSION_ERROR: {
            spdlog::warn("inflateInit: zlib version incompatible with assumed version.");
            break;
        }
        case Z_STREAM_ERROR: {
            spdlog::warn("inflateInit: invalid parameters.");
            break;
        }
        default: {
            break;
        }
    }

    if (ret != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }

    ret = inflate(&stream, Z_FINISH);

    switch (ret) {
        case Z_OK:
        case Z_STREAM_END: {
            break;
        }
        case Z_MEM_ERROR: {
            spdlog::warn("inflate: Not enough memory.");
            break;
        }
        case Z_DATA_ERROR: {
            spdlog::warn("inflate: Input data corrupted. Message: {}", std::string(stream.msg));
            break;
        }
        case Z_STREAM_ERROR: {
            spdlog::warn("inflate: Inconsistent stream structure.");
            break;
        }
        default: {
            break;
        }
    }

    if (stream.msg) {
        spdlog::debug("zlib message: {}", std::string(stream.msg));
    }

    if ((ret != Z_OK) && (ret != Z_BUF_ERROR)) {
        throw std::runtime_error("Decompression failed.");
    }

    ret = inflateEnd(&stream);

    switch (ret) {
        case Z_OK: {
            break;
        }
        case Z_STREAM_ERROR: {
            spdlog::error("inflateEnd: inconsistent stream state.");
            break;
        }
        default: {
            break;
        }
    }

    if (ret != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }

    return buffer;
}
