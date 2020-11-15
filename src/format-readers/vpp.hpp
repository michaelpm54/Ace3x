/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMAT_READERS_VPP_HPP_
#define ACE3X_FORMAT_READERS_VPP_HPP_

#include <cstdint>
#include <string>
#include <vector>

#include "format-readers/archive-entry.hpp"
#include "formats/vpp.hpp"

namespace ace3x::vpp {

struct VppInfo {
    std::uint32_t filenames_offset;
    std::uint32_t data_offset;
    const unsigned char* data;
    VppV2Header header;
    bool compressed;
    std::string filename;
};

inline constexpr std::uint16_t kChunkSize {0x800};
std::uint32_t align_to_chunk(std::uint32_t addr);
std::vector<unsigned char> decompress(const unsigned char* const data, std::uint32_t compressedSize, std::uint32_t uncompressedSize);
VppInfo read_info(const unsigned char* const data, const std::string& filename);
std::vector<ArchiveEntry> read_entries(const VppInfo& info, std::size_t file_size);

}    // namespace ace3x::vpp

#endif    // ACE3X_FORMAT_READERS_VPP_HPP_