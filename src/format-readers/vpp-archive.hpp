/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMAT_READERS_VPP_ARCHIVE_HPP_
#define ACE3X_FORMAT_READERS_VPP_ARCHIVE_HPP_

#include <cstdint>
#include <filesystem>
#include <vector>

class VppArchive {
public:
    ~VppArchive() = default;
    virtual void read(const std::vector<std::uint8_t> &data, const std::filesystem::path &path) = 0;
};

#endif    // ACE3X_FORMAT_READERS_VPP_ARCHIVE_HPP_
