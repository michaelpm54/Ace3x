/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef UTIL_HPP
#define UTIL_HPP

#include <QString>
#include <stdexcept>
#include <string_view>

#include "formats/Peg.hpp"
#include "formats/Vpp.hpp"
#include "tree-entry/tree-entry.hpp"

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string &str);
};

std::string GetExtension(const std::string_view path);
QString getExtension(QString path);
std::vector<std::uint8_t> LoadFile(const std::string &path);
void WriteFile(const std::string &path, const std::vector<std::uint8_t> &buf);

#endif    // UTIL_HPP
