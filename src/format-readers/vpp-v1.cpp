#include "format-readers/vpp-v1.hpp"

#include <filesystem>

#include "file-info.hpp"
#include "format-readers/validation-error.hpp"
#include "format-readers/vpp-common.hpp"
#include "formats/vpp-v1.hpp"

void VppV1::read(const std::vector<std::uint8_t> &data)
{
    VppV1Header header;

    std::memcpy(&header, data.data(), sizeof(VppV1Header));

    if (header.signature != 0x51890ACE) {
        throw ValidationError("signature mismatch");
    }

    if ((header.fileCount <= 0) || (header.fileCount > 5000)) {
        throw ValidationError("bad file count " + std::to_string(header.fileCount));
    }

    std::vector<VppV1DirectoryEntry> dirEntries(header.fileCount);
    std::memcpy(
        dirEntries.data(),
        &data[vpp_common::kChunkSize],
        header.fileCount * sizeof(VppV1DirectoryEntry));

    std::uint32_t dataStart = vpp_common::align_to_chunk(dirEntries.size() * sizeof(VppV1DirectoryEntry) + vpp_common::kChunkSize);

    std::uint32_t offset = dataStart;
    for (std::uint16_t i = 0; i < header.fileCount; i++) {
        const auto filename = dirEntries[i].filename;
        const auto index = i;
        const auto size = dirEntries[i].size;

        std::uint32_t vppOffset = offset;
        offset = vpp_common::align_to_chunk(offset + dirEntries[i].size);

        if (size == 0) {
            // FIXME: Log
            continue;
        }

        if (vppOffset + size >= data.size()) {
            // FIXME: Log
            continue;
        }

        std::vector<std::uint8_t> childData(
            data.begin() + vppOffset,
            data.begin() + vppOffset + size);

        FileInfo info;
        info.index_in_parent = index;
        info.file_name = filename;
        info.file_data = childData;
        info.extension = std::filesystem::path(filename).extension().string();

        entries_.push_back(info);
    }
}

const std::vector<FileInfo> &VppV1::get_entries() const
{
    return entries_;
}
