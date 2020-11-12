/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef UTIL_HPP
#define UTIL_HPP

#include <QString>
#include <stdexcept>
#include <string_view>
#include <vector>

std::vector<std::uint8_t> LoadFile(const std::string &path);
void WriteFile(const std::string &path, const std::vector<std::uint8_t> &buf);

#endif    // UTIL_HPP
