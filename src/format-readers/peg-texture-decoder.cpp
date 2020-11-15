/* SPDX-License-Identifier: GPLv3-or-later */

#include <cstdint>
#include <cstring>

enum FrameFormat {
    PixelFormatRgba5551Indexed = 0x104,    // gggrrrrr abbbbbgg
    PixelFormat0x105 = 0x105,
    PixelFormatRgba32Indexed = 0x204,
};

int munge_palette_index(int value);
std::uint32_t components_to_argb(std::uint8_t a, std::uint8_t r, std::uint8_t g, std::uint8_t b);

void decode_rgba5551_indexed(std::uint32_t *dst, const unsigned char *const src, std::uint16_t width, std::uint16_t height)
{
    static constexpr auto kPaletteEntrySize = 2;
    static constexpr auto kNumPaletteEntries = 256;
    static constexpr auto kPaletteDataSize = kNumPaletteEntries * kPaletteEntrySize;

    std::uint32_t palette[kNumPaletteEntries] = {0};

    for (auto i = 0u, o = 0u; i < kNumPaletteEntries; i++, o += 2u) {
        const auto b0 = src[o + 0u];
        const auto b1 = src[o + 1u];

        const auto r = (b0 & 0x1F) << 3u;
        const auto g = (((b0 & 0xE0) >> 5u) | ((b1 & 0x3) << 3u)) << 3u;
        const auto b = (b1 & 0x7C) << 1u;
        const auto a = (b1 & 0x80) != 0 ? 0xFF : 0x00;

        palette[munge_palette_index(i)] = components_to_argb(a, r, g, b);
    }

    const std::uint32_t size = width * height;
    for (auto i = 0u; i < size; i++)
        dst[i] = palette[src[kPaletteDataSize + i]];
}

void decode_rgba32_indexed(std::uint32_t *dst, const unsigned char *const src, std::uint16_t width, std::uint16_t height)
{
    static constexpr auto kPaletteEntrySize = 4;
    static constexpr auto kNumPaletteEntries = 256;
    static constexpr auto kPaletteDataSize = kNumPaletteEntries * kPaletteEntrySize;

    std::uint32_t palette[kNumPaletteEntries] = {0};

    std::memcpy(palette, src, kPaletteDataSize);

    const std::uint32_t size = width * height;
    for (auto i = 0u; i < size; i++)
        dst[i] = palette[src[kPaletteDataSize + i]] | 0xFF000000;
}

namespace ace3x::peg {

void decode(std::uint32_t *dst, const unsigned char *const src, std::uint16_t width, std::uint16_t height, std::uint16_t format)
{
    switch (format) {
        case PixelFormatRgba5551Indexed:
            decode_rgba5551_indexed(dst, src, width, height);
            break;
        case PixelFormatRgba32Indexed:
            decode_rgba32_indexed(dst, src, width, height);
            break;
        default:
            break;
    }
}

}    // namespace ace3x::peg

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
int munge_palette_index(int value)
{
    // zzzabzzz -> zzzbazzz
    // I'd really like to know WTF.
    return ((value >> 1) & 0x08) | ((value << 1) & 0x10) | (value & 0xE7);
}

std::uint32_t components_to_argb(std::uint8_t a, std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
    /* Automatic integer promotion happens. No need to explicitly cast. */
    return (a << 24) | (r << 16) | (g << 8) | b;
}
