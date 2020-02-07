/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef P3D_HPP
#define P3D_HPP

struct P3DHeader {
        std::uint32_t signature;
        std::uint32_t unk4;
        std::uint32_t size;
        std::uint32_t unkC;
        std::uint32_t unk10;
        std::uint32_t unk14;
        std::uint32_t dataStart;
        std::int32_t numNavpoints;
        std::uint32_t navpointsStart;
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

struct Navpoint { // 34h/52d bytes
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

#endif // P3D_HPP
