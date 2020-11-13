/* SPDX-License-Identifier: GPLv3-or-later */

#include "fs.hpp"

#include <fmt/format.h>

#include <filesystem>
#include <fstream>

namespace ace3x::fs {

std::vector<std::uint8_t> load_file_to_vector(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file) {
        throw std::runtime_error(fmt::format("Failed to open file {} for reading", path));
    }

    auto size = file.tellg();
    file.seekg(0, file.beg);

    std::vector<std::uint8_t> buffer(size);
    file.read((char *)buffer.data(), size);

    return buffer;
}

}    // namespace ace3x::fs
