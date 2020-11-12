/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMAT_READERS_PEG_HPP_
#define ACE3X_FORMAT_READERS_PEG_HPP_

#include <cstdint>
#include <string>
#include <vector>

struct FileInfo;
struct PegFrame;

class Peg {
public:
    struct Image;

    void read(const std::vector<std::uint8_t> &data);
    const std::vector<FileInfo> &get_entries() const;
    Image getImage(std::uint16_t index) const;

private:
    std::vector<FileInfo> entries_;
    std::vector<PegFrame> frames_;
};

struct Peg::Image {
    std::string filename;
    std::vector<std::uint32_t> pixels;
    int width;
    int height;
};

#endif    // ACE3X_FORMAT_READERS_PEG_HPP
