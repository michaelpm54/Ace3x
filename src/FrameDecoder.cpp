/* SPDX-License-Identifier: GPLv3-or-later */

#include "FrameDecoder.hpp"

#include <QDebug>

#include <iostream>
#include <stdexcept>

#include "Entry.hpp"

enum FrameFormat {
	RGBA5551_Indexed = 0x104,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // gggrrrrr abbbbbgg
	UNK0 = 0x105,
	RGBA32_Indexed = 0x204,
};

int      MungePaletteIndex(int value);
uint32_t FromArgb(uint8_t a, uint8_t r, uint8_t g, uint8_t b);

void decode_RGBA5551_Indexed(std::uint32_t *dst, const std::uint8_t *const src, std::uint16_t width, std::uint16_t height)
{
        static constexpr auto PALETTE_ENTRY_SIZE  = 2;
        static constexpr auto NUM_PALETTE_ENTRIES = 256;
        static constexpr auto PALETTE_DATA_SIZE   = NUM_PALETTE_ENTRIES * PALETTE_ENTRY_SIZE;

        std::uint32_t palette[NUM_PALETTE_ENTRIES] = { 0 };

        for (auto i = 0u, o = 0u; i < NUM_PALETTE_ENTRIES; i++, o += 2u) {
                const auto b0 = src[o + 0u];
                const auto b1 = src[o + 1u];

                const auto r = (b0 & 0x1F) << 3u;
                const auto g = (((b0 & 0xE0) >> 5u) | ((b1 & 0x3) << 3u)) << 3u;
                const auto b = (b1 & 0x7C) << 1u;
                const auto a = (b1 & 0x80) != 0 ? 0xFF : 0x00;

                palette[MungePaletteIndex(i)] = FromArgb(a, r, g, b);
        }

        const std::uint32_t size = width * height;
        for (auto i = 0u; i < size; i++)
                dst[i] = palette[src[PALETTE_DATA_SIZE + i]];
}

void decode_RGBA32_Indexed(std::uint32_t *dst, const std::uint8_t *const src, std::uint16_t width, std::uint16_t height)
{
        static constexpr auto PALETTE_ENTRY_SIZE  = 4;
        static constexpr auto NUM_PALETTE_ENTRIES = 256;
        static constexpr auto PALETTE_DATA_SIZE   = NUM_PALETTE_ENTRIES * PALETTE_ENTRY_SIZE;

        std::uint32_t palette[NUM_PALETTE_ENTRIES] = { 0 };

        memcpy(palette, src, PALETTE_DATA_SIZE);

        const std::uint32_t size = width * height;
        for (auto i = 0u; i < size; i++)
                dst[i] = palette[src[PALETTE_DATA_SIZE + i]] | 0xFF000000;
}

void decodeFrame(std::uint32_t *dst, const std::uint8_t *const src, std::uint16_t width, std::uint16_t height, std::uint16_t format)
{
        switch (format) {
                case RGBA5551_Indexed:
                        decode_RGBA5551_Indexed(dst, src, width, height);
                        break;
                case RGBA32_Indexed:
                        decode_RGBA32_Indexed(dst, src, width, height);
                        break;
                default:
                        break;
        }
}

/* The following notice applies to the MungePaletteIndex function directly below,
    as well as the code for decoding format 0x401 (with some variable naming modifications). */
/* Copyright (c) 2017 Rick (rick 'at' gibbed 'dot' us)
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */
int MungePaletteIndex(int value)
{
	// zzzabzzz -> zzzbazzz
	// I'd really like to know WTF.
	return ((value >> 1) & 0x08) | ((value << 1) & 0x10) | (value & 0xE7);
}

uint32_t FromArgb(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
        return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | b;
}
