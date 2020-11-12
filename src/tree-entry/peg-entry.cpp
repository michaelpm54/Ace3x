/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entry/peg-entry.hpp"

#include <QDebug>
#include <QTextEdit>
#include <sstream>

#include "Util.hpp"

void PegEntry::read(std::vector<std::uint8_t> data, QTextEdit *log)
{
    mData = data;

    PegHeader header;
    memcpy(&header, data.data(), sizeof(PegHeader));

    if (header.signature != 0x564B4547) {
        std::stringstream ss;
        ss << "(0x" << std::hex << header.signature << " != 0x564B4547)";
        throw ValidationError("signature mismatch " + ss.str());
    }

    const std::uint32_t headerTotalSize = sizeof(PegHeader) + header.textureHeaderSize;

    mFrames.resize(header.textureCount);

    memcpy(mFrames.data(), &data[sizeof(PegHeader)], sizeof(PegFrame) * header.textureCount);

    auto GetFrameSize = [&](std::uint32_t frameIndex) -> std::uint32_t {
        std::uint32_t frameEnd = 0;

        if (frameIndex + 1 >= header.textureCount)
            frameEnd = headerTotalSize + header.dataSize;
        else
            frameEnd = mFrames[frameIndex + 1].offset;

        return frameEnd - mFrames[frameIndex].offset;
    };

    std::vector<std::uint32_t> sizes(header.textureCount);
    for (std::uint32_t i = 0; i < header.textureCount; i++)
        sizes[i] = GetFrameSize(i);

    for (std::uint32_t i = 0; i < header.textureCount; i++) {
        const auto filename = mFrames[i].filename;
        const auto index = i;
        const auto size = sizes[i];

        if (size > 1000000) {
            if (log)
                log->append(QString("[Warning] Skipping entry %1: size > 1MB").arg(filename));
            continue;
        }

        std::vector<std::uint8_t> childData(
            data.begin() + mFrames[i].offset,
            data.begin() + mFrames[i].offset + sizes[i]);

        FileInfo info;
        info.index_in_parent = index;
        info.file_name = filename;
        info.file_data = data;
        info.file_size = size;

        auto entry = new TreeEntry(info);

        try {
            entry->read(childData);
            addChild(entry);
        }
        catch (const ValidationError &e) {
            std::clog << e.what() << std::endl;
            delete entry;
        }
    }
}

PegFrame PegEntry::getFrame(std::uint16_t index) const
{
    if (index >= mFrames.size())
        return PegFrame();
    return mFrames[index];
}
