/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef UTIL_HPP
#define UTIL_HPP

#include <QString>
#include <stdexcept>
#include <string_view>

#include "Formats/Peg.hpp"
#include "Formats/Vpp.hpp"
#include "tree-entry/tree-entry.hpp"

using ByteVec = std::vector<std::uint8_t>;

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string &str);
};

std::string getExtension(const std::string_view path);
QString getExtension(QString path);
ByteVec LoadFile(const std::string &path);
void WriteFile(const std::string &path, const ByteVec &buf);

#endif    // UTIL_HPP
