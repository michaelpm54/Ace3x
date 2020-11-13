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

void VppV2::read(const std::vector<std::uint8_t> &data, const std::filesystem::path &path)
{
    VppV2Header header;

    std::memcpy(&header, data.data(), sizeof(VppV2Header));

    if (header.signature != 0x51890ACE) {
        throw ValidationError("signature mismatch");
    }

    if ((header.fileCount <= 0) || (header.fileCount > 5000)) {
        throw ValidationError("bad file count " + std::to_string(header.fileCount));
    }

    if (header.version != 2) {
        throw ValidationError("bad version");
    }

    std::vector<VppV2DirectoryEntry> dirEntries(header.fileCount);
    std::memcpy(
        dirEntries.data(),
        &data[vpp_common::kChunkSize],
        header.fileCount * sizeof(VppV2DirectoryEntry));

    compressed_ = header.compressedDataSize != 0xFFFFFFFF;

    const std::uint32_t filenamesStart = vpp_common::align_to_chunk(vpp_common::kChunkSize + header.directorySize);
    std::vector<std::string> filenames;
    {
        std::ptrdiff_t offset = 0;
        do {
            filenames.push_back(std::string(reinterpret_cast<const char *>(&data[filenamesStart] + offset)));
            offset += filenames.back().size() + 1;
        } while (offset < header.filenamesSize);
    }

    std::uint32_t dataStart = vpp_common::align_to_chunk(filenamesStart + header.filenamesSize);

    std::vector<std::uint8_t> decompressed_data;

    if (compressed_) {
        decompressed_data = decompress(&data[dataStart], header.compressedDataSize, header.uncompressedDataSize);
        dataStart = 0;
    }
    else {
        decompressed_data = data;
    }

    const auto vpp_name = path.filename().string();

    std::uint32_t offset = dataStart;
    for (std::uint16_t i = 0; i < header.fileCount; i++) {
        const auto filename = filenames[i];
        const auto index = i;
        const auto size = dirEntries[i].uncompressedSize;

        std::uint32_t vppOffset = compressed_ ? dirEntries[i].rtOffset : offset;
        if (!compressed_) {
            offset = vpp_common::align_to_chunk(offset + dirEntries[i].uncompressedSize);
        }

        if (size == 0) {
            spdlog::warn("'{}/{}' size is 0, skipping", vpp_name, filename);
            continue;
        }

        if (vppOffset + size >= decompressed_data.size()) {
            spdlog::warn("'{}/{}' exceeds data size, skipping", vpp_name, filename);
            continue;
        }

        std::vector<std::uint8_t> childData(
            decompressed_data.begin() + vppOffset,
            decompressed_data.begin() + vppOffset + size);

        FileInfo info;
        info.index_in_parent = index;
        info.file_name = filename;
        info.absolute_path = path.string() + '/' + info.file_name;
        info.extension = std::filesystem::path(filename).extension().string();
        info.file_data = childData;

        entries_.push_back(info);
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
