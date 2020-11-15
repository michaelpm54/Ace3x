/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMAT_READERS_PEG_TEXTURE_DECODER_HPP_
#define ACE3X_FORMAT_READERS_PEG_TEXTURE_DECODER_HPP_

#include <cstdint>

namespace ace3x::peg {

void decode(std::uint32_t *dst, const unsigned char *const src, std::uint16_t width, std::uint16_t height, std::uint16_t format);

}

#endif    // ACE3X_FORMAT_READERS_PEG_TEXTURE_DECODER_HPP_
