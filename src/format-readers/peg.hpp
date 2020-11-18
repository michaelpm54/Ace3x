#ifndef ACE3X_FORMAT_READERS_PEG_HPP_
#define ACE3X_FORMAT_READERS_PEG_HPP_

#include <string>
#include <vector>

#include "format-readers/archive-entry.hpp"

namespace ace3x::peg {

struct Image {
    std::string filename;
    std::vector<std::uint32_t> pixels;
    int width;
    int height;
    std::uint16_t format;
};

std::vector<ArchiveEntry> read_entries(const unsigned char *const data, const std::string &peg_name);
std::vector<Image> get_images(const unsigned char *const data);

}    // namespace ace3x::peg

#endif    // ACE3X_FORMAT_READERS_PEG_HPP_
