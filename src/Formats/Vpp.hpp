/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef VPP_HPP
#define VPP_HPP

struct VppHeader_V1 {
        std::uint32_t signature;
        std::uint32_t version;
        std::uint32_t fileCount;
        std::uint32_t fileSize;
};

struct VppDirectoryEntry_V1 {
        char filename[0x3C];
        std::uint32_t size;
};

struct VppHeader_V2 {
        uint32_t signature;                                // 0x0
        uint32_t version;                                  // 0x4
        char     name[44];                                 // 0x8
        uint32_t unk0;                                     // 0x34
        uint32_t unk1;                                     // 0x38
        uint32_t fileCount;                                // 0x3C
        uint32_t vppSize;                                  // 0x40
        uint32_t directorySize;                            // 0x44
        uint32_t filenamesSize;                            // 0x48
        uint32_t compressedDataSize;                       // 0x4C
        uint32_t uncompressedDataSize;                     // 0x50
        uint32_t unk2;                                     // 0x54
        uint32_t unk3;                                     // 0x58
};

struct VppDirectoryEntry_V2 {
        uint32_t filenameEnd;
        uint32_t rtOffset;
        uint32_t nameHash;
        uint32_t uncompressedSize;
        uint32_t compressedSize;
        uint32_t pkgPtr;
        uint32_t unk2;
};

#endif // VPP_HPP
