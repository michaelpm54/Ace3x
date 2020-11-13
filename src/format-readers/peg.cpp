/* SPDX-License-Identifier: GPLv3-or-later */

#include "format-readers/peg.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>

#include "file-info.hpp"
#include "format-readers/peg-texture-decoder.hpp"
#include "format-readers/validation-error.hpp"
#include "formats/peg.hpp"

void Peg::read(const FileInfo &info)
{
    PegHeader header;
    std::memcpy(&header, info.mmap.data(), sizeof(PegHeader));

    if (header.signature != 0x564B4547) {
        const auto msg = fmt::format("Failed loading PEG '{}': signature 0x{} != 0x564B4547", info.file_name, header.signature);
        spdlog::error(msg);
        throw ValidationError("signature mismatch: " + msg);
    }

    const std::uint32_t headerTotalSize = sizeof(PegHeader) + header.textureHeaderSize;

    frames_.resize(header.textureCount);

    std::memcpy(frames_.data(), &info.mmap.data()[sizeof(PegHeader)], sizeof(PegFrame) * header.textureCount);

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
            spdlog::error("PEG '{}': frame {} has invalid filename", info.file_name, i);
            continue;
        }

        const auto index = i;
        const auto size = sizes[i];

        if (size > 1000000) {
            spdlog::warn("PEG '{}': Skipping entry '{}' because size ({} bytes) is > 1MB", info.file_name, filename, size);
            continue;
        }

        FileInfo child;
        child.index_in_parent = index;
        child.file_name = filename;

        std::error_code error;
        child.mmap = mio::make_mmap_source(info.base_file_absolute_path, info.offset + frames_[i].offset, size, error);

        if (error) {
            spdlog::error("Failed to map '{}/{}': {}", info.file_name, filename, error.message());
            continue;
        }

        entries_.push_back(child);
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
    peg_texture_decoder::decode(pixels.data(), reinterpret_cast<const std::uint8_t *>(entries_[index].mmap.data()), frame.width, frame.height, frame.format);

    image.filename = frame.filename;
    image.pixels = pixels;
    image.width = frame.width;
    image.height = frame.height;

    return image;
}
