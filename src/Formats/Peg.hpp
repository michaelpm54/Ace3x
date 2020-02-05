/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef PEG_HPP
#define PEG_HPP

struct PegHeader {
	uint32_t signature;
	uint32_t version;
	uint32_t textureHeaderSize;
	uint32_t dataSize;
	uint32_t textureCount{ 0 };
	uint32_t unk14; // always 0x0
	uint32_t frameCount{ 0 };
	uint32_t unk1C; // always 0x10
};

struct PegFrame {
        uint16_t width { 0 };
	uint16_t height;
	uint16_t format;
	uint16_t unk1;
	uint16_t unk2;
	uint16_t unk3;
	char     filename[48]; // uint32_t[12]
	uint32_t offset;
};

#endif // PEG_HPP
