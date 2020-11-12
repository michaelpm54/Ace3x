/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMATS_P3D_HPP_
#define ACE3X_FORMATS_P3D_HPP_

#include <cstdint>

struct P3DHeader {
    std::uint32_t signature;
    std::uint32_t unk4;
    std::uint32_t size;
    std::uint32_t numUnkC;
    std::uint32_t subHeadersEnd;
    std::uint32_t numSubHeaders;
    std::uint32_t filenamesEnd;
    std::int32_t numNavpoints;
    std::int32_t navpointsStart;
    std::uint32_t unk24;
    std::uint32_t unk28;
    std::uint32_t unk2C;
    std::uint32_t unk30;
    std::uint32_t unk34;
    std::uint32_t unk38;
    std::uint32_t unk3C;
    std::int32_t numImages;
    std::int32_t imagesStart;
    std::uint32_t unk48;
    std::uint32_t unk4C;
    std::uint32_t unk50;
    std::uint32_t unk54;
    std::uint32_t unk58;
};

struct P3DSubHeader {
    std::uint32_t unk0;
    std::uint32_t unk4;
    std::uint32_t offset8;
    std::uint32_t offsetC;
    std::uint32_t offset10;
    std::uint32_t offset14;
    std::uint32_t unk18;
};

struct Navpoint {    // 34h/52d bytes
    std::uint8_t unk0[4];
    float unk4;
    std::uint8_t unk8[12];
    float unk14;
    std::uint8_t unk18[12];
    float unk24;
    float x;
    float y;
    float z;
};

#endif    // ACE3X_FORMATS_P3D_HPP_
