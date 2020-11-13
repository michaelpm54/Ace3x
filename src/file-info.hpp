/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FILE_INFO_HPP_
#define ACE3X_FILE_INFO_HPP_

#include <string>

#include "mio.hpp"

struct FileInfo {
    int index_in_parent {-1};
    std::string file_name;
    std::string absolute_path;
    std::string extension;
    std::string base_file_absolute_path;
    std::int64_t offset;
    mio::shared_mmap_source mmap;
};

#endif    // ACE3X_FILE_INFO_HPP_
