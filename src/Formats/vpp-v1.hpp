#ifndef ACE3X_FORMATS_VPP_V1_HEADER_HPP_
#define ACE3X_FORMATS_VPP_V1_HEADER_HPP_

#include <cstdint>

struct VppV1Header {
    std::uint32_t signature;
    std::uint32_t version;
    std::uint32_t fileCount;
    std::uint32_t fileSize;
};

struct VppV1DirectoryEntry {
    char filename[0x3C];
    std::uint32_t size;
};

#endif    // ACE3X_FORMATS_VPP_V1_HEADER_HPP_
