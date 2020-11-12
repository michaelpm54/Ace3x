/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMATS_PEG_HPP_
#define ACE3X_FORMATS_PEG_HPP_

#include <cstdint>

struct PegHeader {
    std::uint32_t signature;
    std::uint32_t version;
    std::uint32_t textureHeaderSize;
    std::uint32_t dataSize;
    std::uint32_t textureCount {0};
    std::uint32_t unk14;    // always 0x0
    std::uint32_t frameCount {0};
    std::uint32_t unk1C;    // always 0x10
};

struct PegFrame {
    std::uint16_t width {0};
    std::uint16_t height;
    std::uint16_t format;
    std::uint16_t unk1;
    std::uint16_t unk2;
    std::uint16_t unk3;
    char filename[48];    // uint32_t[12]
    std::uint32_t offset;
};

#endif    // ACE3X_FORMATS_PEG_HPP_
