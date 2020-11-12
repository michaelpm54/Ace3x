/* SPDX-License-Identifier: GPLv3-or-later */

#include "fs.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace ace3x::fs {

std::vector<std::uint8_t> load_file_to_vector(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    // FIXME: Log
    if (!file) {
        std::stringstream ss;
        ss << "Failed to open file '" << path << "' for reading.";
        throw std::runtime_error(ss.str());
    }

    auto size = file.tellg();
    file.seekg(0, file.beg);

    std::vector<std::uint8_t> buffer(size);
    file.read((char *)buffer.data(), size);

    return buffer;
}

}    // namespace ace3x::fs
