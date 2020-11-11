/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef FRAME_DECODER_H
#define FRAME_DECODER_H

#include <cstdint>

struct Image {
    const void *pixels;
    std::uint64_t size {0};
    std::uint16_t width;
    std::uint16_t height;
};

void decodeFrame(std::uint32_t *dst, const std::uint8_t *const src, std::uint16_t width, std::uint16_t height, std::uint16_t format);

#endif    // FRAME_DECODER_H
