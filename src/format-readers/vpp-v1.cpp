/* SPDX-License-Identifier: GPLv3-or-later */

#include "format-readers/vpp-v1.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>

#include "file-info.hpp"
#include "format-readers/validation-error.hpp"
#include "format-readers/vpp-common.hpp"
#include "formats/vpp-v1.hpp"

void VppV1::read(const FileInfo &info)
{
    VppV1Header header;

    std::memcpy(&header, info.mmap.data(), sizeof(VppV1Header));

    if (header.signature != 0x51890ACE) {
        throw ValidationError("signature mismatch");
    }

    if ((header.fileCount <= 0) || (header.fileCount > 5000)) {
        throw ValidationError("bad file count " + std::to_string(header.fileCount));
    }

    std::vector<VppV1DirectoryEntry> dirEntries(header.fileCount);
    std::memcpy(
        dirEntries.data(),
        &info.mmap.data()[vpp_common::kChunkSize],
        header.fileCount * sizeof(VppV1DirectoryEntry));

    std::uint32_t dataStart = vpp_common::align_to_chunk(static_cast<std::uint32_t>(dirEntries.size() * sizeof(VppV1DirectoryEntry) + vpp_common::kChunkSize));

    std::uint32_t offset = dataStart;
    for (std::uint16_t i = 0; i < header.fileCount; i++) {
        const auto filename = dirEntries[i].filename;
        const auto index = i;
        const auto size = dirEntries[i].size;

        std::uint32_t vppOffset = offset;
        offset = vpp_common::align_to_chunk(offset + dirEntries[i].size);

        if (size == 0) {
            spdlog::warn("'{}/{}' size is 0, skipping", info.file_name, filename);
            continue;
        }

        if (vppOffset + size >= info.mmap.size()) {
            spdlog::warn("'{}/{}' exceeds data size, skipping", info.file_name, filename);
            continue;
        }

        FileInfo child;
        child.index_in_parent = index;
        child.file_name = filename;
        child.extension = std::filesystem::path(filename).extension().string();
        child.absolute_path = info.absolute_path + '/' + child.file_name;

        std::error_code error;
        child.mmap = mio::make_mmap_source(info.mmap, vppOffset, size, error);

        if (error) {
            spdlog::error("Failed to map '{}/{}': {}", info.file_name, filename, error.message());
            continue;
        }

        entries_.push_back(child);
    }
}

const std::vector<FileInfo> &VppV1::get_entries() const
{
    return entries_;
}
