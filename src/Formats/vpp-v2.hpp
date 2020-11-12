#ifndef ACE3X_FORMATS_VPP_V2_HEADER_HPP_
#define ACE3X_FORMATS_VPP_V2_HEADER_HPP_

#include <cstdint>

struct VppV2Header {
    std::uint32_t signature;               // 0x0
    std::uint32_t version;                 // 0x4
    char name[44];                         // 0x8
    std::uint32_t unk0;                    // 0x34
    std::uint32_t unk1;                    // 0x38
    std::uint32_t fileCount;               // 0x3C
    std::uint32_t vppSize;                 // 0x40
    std::uint32_t directorySize;           // 0x44
    std::uint32_t filenamesSize;           // 0x48
    std::uint32_t compressedDataSize;      // 0x4C
    std::uint32_t uncompressedDataSize;    // 0x50
    std::uint32_t unk2;                    // 0x54
    std::uint32_t unk3;                    // 0x58
};

struct VppV2DirectoryEntry {
    std::uint32_t filenameEnd;
    std::uint32_t rtOffset;
    std::uint32_t nameHash;
    std::uint32_t uncompressedSize;
    std::uint32_t compressedSize;
    std::uint32_t pkgPtr;
    std::uint32_t unk2;
};

#endif    // ACE3X_FORMATS_VPP_V2_HEADER_HPP_
