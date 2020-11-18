#include "format-readers/vpp.hpp"

#include <spdlog/spdlog.h>
#include <zlib.h>

#include "format-readers/validation-error.hpp"

namespace ace3x::vpp {

std::uint32_t align_to_chunk(std::uint32_t addr)
{
    if (addr == 0) {
        return addr;
    }

    int left = addr % kChunkSize;

    if (left > 0) {
        return (addr + (kChunkSize - left));
    }

    return addr;
}

std::vector<unsigned char> decompress(const unsigned char* const data, std::uint32_t compressedSize, std::uint32_t uncompressedSize)
{
    std::vector<unsigned char> buffer(uncompressedSize);

    z_stream stream;
    stream.next_in = const_cast<unsigned char*>(data);
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

VppInfo read_info(const unsigned char* const data, const std::string& filename)
{
    VppInfo info;

    info.data = data;
    info.filename = filename;

    std::memcpy(&info.header, data, sizeof(info.header));

    if (info.header.signature != 0x51890ACE) {
        throw ValidationError("signature mismatch");
    }

    if ((info.header.fileCount <= 0) || (info.header.fileCount > 5000)) {
        throw ValidationError("too many files " + std::to_string(info.header.fileCount));
    }

    if (info.header.version != 2) {
        throw ValidationError("bad version");
    }

    info.compressed = info.header.compressedDataSize != static_cast<std::uint32_t>(-1);

    info.filenames_offset = align_to_chunk(kChunkSize + info.header.directorySize);
    info.data_offset = align_to_chunk(info.filenames_offset + info.header.filenamesSize);

    return info;
}

std::vector<ArchiveEntry> read_entries(const VppInfo& info, std::size_t file_size)
{
    /* Read file list */
    std::vector<VppV2DirectoryEntry> dir_entries(info.header.fileCount);
    std::memcpy(
        dir_entries.data(),
        &info.data[ace3x::vpp::kChunkSize],
        info.header.fileCount * sizeof(VppV2DirectoryEntry));

    /* Read filenames */
    const std::uint32_t filenamesStart = ace3x::vpp::align_to_chunk(ace3x::vpp::kChunkSize + info.header.directorySize);
    std::vector<std::string> filenames;
    {
        std::ptrdiff_t offset = 0;
        do {
            filenames.push_back(std::string(reinterpret_cast<const char*>(&info.data[filenamesStart] + offset)));
            offset += filenames.back().size() + 1;
        } while (offset < info.header.filenamesSize);
    }

    auto offset = info.data_offset;

    std::vector<ArchiveEntry> entries;

    for (std::uint16_t i = 0; i < info.header.fileCount; i++) {
        const std::string filename = filenames[i];
        const int size = dir_entries[i].uncompressedSize;
        const int final_offset = info.compressed ? dir_entries[i].offset + info.data_offset : offset;

        if (size == 0) {
            spdlog::warn("VPP: Skipping entry '{}/{}' because size is 0", filename, filename);
            continue;
        }

        if (static_cast<uint64_t>(final_offset) + size >= file_size) {
            spdlog::warn("VPP: Skipping entry '{}/{}' because [offset 0x{:04x} + size 0x{:04x} = 0x{:04x}] exceeds data size 0x{:04x}", info.filename, filename, offset, size, offset + size, file_size);
            continue;
        }

        ArchiveEntry entry;
        entry.filename = filename;
        entry.index = i;
        entry.size = size;
        entry.offset = final_offset;

        offset = align_to_chunk(offset + dir_entries[i].uncompressedSize);

        entries.push_back(entry);
    }

    return entries;
}

}    // namespace ace3x::vpp
