/* SPDX-License-Identifier: GPLv3-or-later */

#include "format-readers/peg.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>

#include "file-info.hpp"
#include "format-readers/peg-texture-decoder.hpp"
#include "format-readers/validation-error.hpp"
#include "formats/peg.hpp"

void Peg::read(const std::vector<std::uint8_t> &data, const std::filesystem::path &path)
{
    PegHeader header;
    std::memcpy(&header, data.data(), sizeof(PegHeader));

    const auto peg_name = path.filename().string();

    if (header.signature != 0x564B4547) {
        const auto msg = fmt::format("Failed loading PEG '{}': signature 0x{} != 0x564B4547", peg_name, header.signature);
        spdlog::error(msg);
        throw ValidationError("signature mismatch: " + msg);
    }

    const std::uint32_t headerTotalSize = sizeof(PegHeader) + header.textureHeaderSize;

    frames_.resize(header.textureCount);

    std::memcpy(frames_.data(), &data.data()[sizeof(PegHeader)], sizeof(PegFrame) * header.textureCount);

    auto GetFrameSize = [&](std::uint32_t frameIndex) -> std::uint32_t {
        std::uint32_t frameEnd = 0;

        if (frameIndex + 1 >= header.textureCount)
            frameEnd = headerTotalSize + header.dataSize;
        else
            frameEnd = frames_[frameIndex + 1].offset;

        return frameEnd - frames_[frameIndex].offset;
    };

    std::vector<std::uint32_t> sizes(header.textureCount);
    for (std::uint32_t i = 0; i < header.textureCount; i++)
        sizes[i] = GetFrameSize(i);

    for (std::uint32_t i = 0; i < header.textureCount; i++) {
        const std::string filename = frames_[i].filename;

        if (std::any_of(std::begin(filename), std::end(filename), [](char c) {
                return static_cast<unsigned char>(c) > 127;
            }) ||
            filename.size() == 4 /* just an extension */) {
            spdlog::error("PEG '{}': frame {} has invalid filename", peg_name, i);
            continue;
        }

        const auto index = i;
        const auto size = sizes[i];

        if (size > 1000000) {
            spdlog::warn("PEG '{}': Skipping entry '{}' because size ({} bytes) is > 1MB", peg_name, filename, size);
            continue;
        }

        std::vector<std::uint8_t> childData(
            data.begin() + frames_[i].offset,
            data.begin() + frames_[i].offset + sizes[i]);

        FileInfo info;
        info.index_in_parent = index;
        info.file_name = filename;
        info.file_data = childData;

        entries_.push_back(info);
    }
}

const std::vector<FileInfo> &Peg::get_entries() const
{
    return entries_;
}

Peg::Image Peg::getImage(std::uint16_t index) const
{
    Peg::Image image;
    image.pixels = {};
    image.width = 0;
    image.height = 0;

    if (index >= frames_.size()) {
        return image;
    }

    auto frame = frames_[index];

    std::vector<std::uint32_t> pixels(frame.width * frame.height);
    peg_texture_decoder::decode(pixels.data(), entries_[index].file_data.data(), frame.width, frame.height, frame.format);

    image.filename = frame.filename;
    image.pixels = pixels;
    image.width = frame.width;
    image.height = frame.height;

    return image;
}
