#include "format-readers/peg.hpp"

#include <spdlog/spdlog.h>

#include <QLocale>

#include "format-readers/peg-texture-decoder.hpp"
#include "format-readers/validation-error.hpp"
#include "formats/peg.hpp"

namespace ace3x::peg {

std::vector<ArchiveEntry> read_entries(const unsigned char *const data, const std::string &peg_name)
{
    std::vector<ArchiveEntry> entries;

    PegHeader header;
    std::memcpy(&header, data, sizeof(PegHeader));

    if (header.signature != 0x564B4547) {
        throw ValidationError(fmt::format("PEG '{}': Signature mismatch 0x{} != 0x564B4547", peg_name, header.signature));
    }

    const std::uint32_t headerTotalSize = sizeof(PegHeader) + header.textureHeaderSize;

    std::vector<PegFrame> frames(header.textureCount);

    std::memcpy(frames.data(), &data[sizeof(PegHeader)], sizeof(PegFrame) * header.textureCount);

    auto GetFrameSize = [&](std::uint64_t frameIndex) -> std::uint32_t {
        std::uint32_t frameEnd = 0;

        if (frameIndex + 1 >= header.textureCount)
            frameEnd = headerTotalSize + header.dataSize;
        else
            frameEnd = frames[frameIndex + 1].offset;

        return frameEnd - frames[frameIndex].offset;
    };

    std::vector<std::uint32_t> sizes(header.textureCount);
    for (std::uint32_t i = 0; i < header.textureCount; i++)
        sizes[i] = GetFrameSize(i);

    for (std::uint32_t i = 0; i < header.textureCount; i++) {
        const std::string filename = frames[i].filename;

        if (std::any_of(std::begin(filename), std::end(filename), [](char c) {
                return static_cast<unsigned char>(c) > 127;
            }) ||
            filename.size() == 4 /* just an extension */) {
            spdlog::warn("PEG: Skipping entry '{}/({}) because the filename is invalid", peg_name, i);
            continue;
        }

        if (sizes[i] > 1000000) {
            spdlog::warn("PEG: Skipping entry '{}/{}' because it is too large ({} > 1MB)", peg_name, filename, QLocale::system().formattedDataSize(sizes[i], 2, nullptr).toStdString());
            continue;
        }

        ArchiveEntry entry;
        entry.index = i;
        entry.size = sizes[i];
        entry.offset = frames[i].offset;
        entry.filename = filename;

        entries.push_back(entry);
    }

    return entries;
}

std::vector<Image> get_images(const unsigned char *const data)
{
    PegHeader header;
    std::memcpy(&header, data, sizeof(PegHeader));

    const std::uint32_t headerTotalSize = sizeof(PegHeader) + header.textureHeaderSize;

    std::vector<PegFrame> frames(header.textureCount);

    std::memcpy(frames.data(), &data[sizeof(PegHeader)], sizeof(PegFrame) * header.textureCount);

    auto GetFrameSize = [&](std::uint64_t frameIndex) -> std::uint32_t {
        std::uint32_t frameEnd = 0;

        if (frameIndex + 1 >= header.textureCount)
            frameEnd = headerTotalSize + header.dataSize;
        else
            frameEnd = frames[frameIndex + 1].offset;

        return frameEnd - frames[frameIndex].offset;
    };

    std::vector<std::uint32_t> sizes(header.textureCount);
    for (std::uint32_t i = 0; i < header.textureCount; i++)
        sizes[i] = GetFrameSize(i);

    std::vector<Image> images;

    for (int i = 0; i < header.textureCount; i++) {
        const auto &frame {frames[i]};

        Image image;
        image.width = frame.width;
        image.height = frame.height;
        image.filename = frame.filename;
        image.pixels.resize(static_cast<std::uint64_t>(frame.width) * static_cast<uint64_t>(frame.height));
        ace3x::peg::decode(image.pixels.data(), reinterpret_cast<const unsigned char *>(data + frame.offset), frame.width, frame.height, frame.format);

        images.push_back(image);
    }

    return images;
}

}    // namespace ace3x::peg
