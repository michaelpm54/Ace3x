#ifndef ACE3X_FORMAT_READERS_PEG_TEXTURE_DECODER_HPP_
#define ACE3X_FORMAT_READERS_PEG_TEXTURE_DECODER_HPP_

#include <cstdint>

namespace peg_texture_decoder {

void decode(std::uint32_t *dst, const std::uint8_t *const src, std::uint16_t width, std::uint16_t height, std::uint16_t format);

}

#endif    // ACE3X_FORMAT_READERS_PEG_TEXTURE_DECODER_HPP_
