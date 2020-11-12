/* SPDX-License-Identifier: GPLv3-or-later */

#include "Util.hpp"

#include <QFileInfo>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

ValidationError::ValidationError(const std::string &str)
    : std::runtime_error("Validation failed: " + str)
{
}

std::string GetExtension(const std::string_view path)
{
    return std::filesystem::path(path).extension().string();
}

QString getExtension(QString path)
{
    return QFileInfo(path.toLower()).suffix();
}

std::vector<std::uint8_t> LoadFile(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
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

void WriteFile(const std::string &path, const std::vector<std::uint8_t> &buf)
{
    std::string filename = std::filesystem::path(path).filename().string();
    filename = "uncomp_" + filename;

    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
        throw std::runtime_error("Failed to open " + filename + " for writing.");

    file.write((char *)buf.data(), buf.size());
    file.flush();
    file.close();

    std::clog << "Wrote " << filename << " successfully." << std::endl;
}
