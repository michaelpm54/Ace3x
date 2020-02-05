/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef P3D_HPP
#define P3D_HPP

struct P3DHeader {
        std::uint32_t signature;
        std::uint32_t version;
        std::uint32_t size;
        std::uint32_t unkC;
        std::uint32_t unk10;
        std::uint32_t unk14;
        std::uint32_t dataStart;
        std::uint32_t unk1C;
        std::uint32_t unk20;
        std::uint32_t unk24;
        std::uint32_t unk28;
        std::uint32_t unk2C;
        std::uint32_t unk30;
        std::uint32_t unk34;
        std::uint32_t unk38;
        std::uint32_t unk3C;
        std::uint32_t unk40;
        std::uint32_t unk44;
        std::uint32_t unk48;
        std::uint32_t unk4C;
        std::uint32_t unk50;
        std::uint32_t unk54;
        std::uint32_t unk58;
};

#endif // P3D_HPP
