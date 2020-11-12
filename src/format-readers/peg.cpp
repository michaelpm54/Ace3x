#include "format-readers/peg.hpp"

#include <sstream>

#include "file-info.hpp"
#include "format-readers/peg-texture-decoder.hpp"
#include "format-readers/validation-error.hpp"
#include "formats/peg.hpp"

void Peg::read(const std::vector<std::uint8_t> &data)
{
    PegHeader header;
    std::memcpy(&header, data.data(), sizeof(PegHeader));

    // FIXME: Log
    if (header.signature != 0x564B4547) {
        std::stringstream ss;
        ss << "(0x" << std::hex << header.signature << " != 0x564B4547)";
        throw ValidationError("signature mismatch " + ss.str());
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
        const auto filename = frames_[i].filename;
        const auto index = i;
        const auto size = sizes[i];

        if (size > 1000000) {
            // FIXME
            /*
            if (log)
                log->append(QString("[Warning] Skipping entry %1: size > 1MB").arg(filename));
			*/
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
