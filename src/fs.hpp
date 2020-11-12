/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FS_HPP_
#define ACE3X_FS_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace ace3x::fs {

std::vector<std::uint8_t> load_file_to_vector(const std::string &path);

}

#endif    // ACE3X_FS_HPP_
