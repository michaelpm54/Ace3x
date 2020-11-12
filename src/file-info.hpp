/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FILE_INFO_HPP_
#define ACE3X_FILE_INFO_HPP_

#include <cstdint>
#include <string>
#include <vector>

struct FileInfo {
    int index_in_parent;
    std::string file_name;
    std::string absolute_path;
    std::string extension;
    // FIXME: This uses more memory than necessary
    // Replace it with a pointer
    std::vector<std::uint8_t> file_data;
};

#endif    // ACE3X_FILE_INFO_HPP_
