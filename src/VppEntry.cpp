/* SPDX-License-Identifier: GPLv3-or-later */

#include "VppEntry.hpp"

#include <zlib.h>

#include <QDebug>
#include <QTextEdit>

#include "PegEntry.hpp"
#include "Util.hpp"

static constexpr std::uint16_t CHUNK_SIZE = 0x800;

uint32_t alignToChunk(int n);
std::vector<std::uint8_t> decompress(const std::uint8_t *const data, std::uint32_t compressedSize, std::uint32_t uncompressedSize);

VppEntry::VppEntry(QString name)
    : Entry(name)
{
}

void VppEntry::readVpp1(std::vector<std::uint8_t> &data, QTextEdit *log)
{
    VppHeader_V1 header;

    memcpy(&header, data.data(), sizeof(VppHeader_V1));

    if (header.signature != 0x51890ACE) {
        throw ValidationError("signature mismatch");
    }

    if ((header.fileCount <= 0) || (header.fileCount > 5000)) {
        throw ValidationError("bad file count " + std::to_string(header.fileCount));
    }

    std::vector<VppDirectoryEntry_V1> dirEntries(header.fileCount);
    memcpy(
        dirEntries.data(),
        &data[CHUNK_SIZE],
        header.fileCount * sizeof(VppDirectoryEntry_V1));

    std::uint32_t dataStart = alignToChunk(dirEntries.size() * sizeof(VppDirectoryEntry_V1) + 0x800);

    std::uint32_t offset = dataStart;
    for (std::uint16_t i = 0; i < header.fileCount; i++) {
        const auto filename = QString::fromStdString(dirEntries[i].filename);
        const auto index = i;
        const auto size = dirEntries[i].size;

        std::uint32_t vppOffset = offset;
        offset = alignToChunk(offset + dirEntries[i].size);

        if (size == 0) {
            if (log) {
                log->append(QString("[Warning] Skipping entry %1: size 0").arg(filename));
            }
            continue;
        }

        if (vppOffset + size >= data.size()) {
            if (log) {
                log->append(QString("[Warning] Skipping entry %1: offset exceeds data size").arg(filename));
            }
            continue;
        }

        const auto ext = ::getExtension(filename);

        Entry *entry;

        if (ext == "peg") {
            entry = new PegEntry(filename, index, size);
        }
        else {
            entry = new Entry(filename, index, size);
        }

        std::vector<std::uint8_t> childData(
            data.begin() + vppOffset,
            data.begin() + vppOffset + size);

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

void VppEntry::readVpp2(std::vector<std::uint8_t> &data, QTextEdit *log)
{
    VppHeader_V2 header;

    memcpy(&header, data.data(), sizeof(VppHeader_V2));

    if (header.signature != 0x51890ACE) {
        throw ValidationError("signature mismatch");
    }

    if ((header.fileCount <= 0) || (header.fileCount > 5000)) {
        throw ValidationError("bad file count " + std::to_string(header.fileCount));
    }

    if (header.version != 2) {
        throw ValidationError("bad version");
    }

    std::vector<VppDirectoryEntry_V2> dirEntries(header.fileCount);
    memcpy(
        dirEntries.data(),
        &data[CHUNK_SIZE],
        header.fileCount * sizeof(VppDirectoryEntry_V2));

    mCompressed = header.compressedDataSize != 0xFFFFFFFF;

    const std::uint32_t filenamesStart = alignToChunk(CHUNK_SIZE + header.directorySize);
    std::vector<std::string> filenames;
    {
        std::ptrdiff_t offset = 0;
        do {
            filenames.push_back(std::string(reinterpret_cast<const char *>(&data[filenamesStart] + offset)));
            offset += filenames.back().size() + 1;
        } while (offset < header.filenamesSize);
    }

    std::uint32_t dataStart = alignToChunk(filenamesStart + header.filenamesSize);

    if (mCompressed) {
        data = decompress(&data[dataStart], header.compressedDataSize, header.uncompressedDataSize);
        dataStart = 0;
    }

    std::uint32_t offset = dataStart;
    for (std::uint16_t i = 0; i < header.fileCount; i++) {
        const auto filename = QString::fromStdString(filenames[i]);
        const auto index = i;
        const auto size = dirEntries[i].uncompressedSize;

        std::uint32_t vppOffset = mCompressed ? dirEntries[i].rtOffset : offset;
        if (!mCompressed) {
            offset = alignToChunk(offset + dirEntries[i].uncompressedSize);
        }

        if (size == 0) {
            if (log) {
                log->append(QString("[Warning] Skipping entry %1: size 0").arg(filename));
            }
            continue;
        }

        if (vppOffset + size >= data.size()) {
            if (log) {
                log->append(QString("[Warning] Skipping entry %1: offset exceeds data size").arg(filename));
            }
            continue;
        }

        const auto ext = ::getExtension(filename);

        Entry *entry;

        if (ext == "peg") {
            entry = new PegEntry(filename, index, size);
        }
        else {
            entry = new Entry(filename, index, size);
        }

        std::vector<std::uint8_t> childData(
            data.begin() + vppOffset,
            data.begin() + vppOffset + size);

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

void VppEntry::read(std::vector<std::uint8_t> data, QTextEdit *log)
{
    mSize = data.size();

    std::uint32_t version = 0;
    memcpy(&version, &data[0x4], 4);

    switch (version) {
        case 1: {
            readVpp1(data, log);
            break;
        }
        case 2: {
            readVpp2(data, log);
            break;
        }
        default: {
            throw std::runtime_error("Invalid VPP version");
            break;
        }
    }

    mData = data;
}

std::vector<std::uint8_t> decompress(const std::uint8_t *const data, std::uint32_t compressedSize, std::uint32_t uncompressedSize)
{
    std::vector<std::uint8_t> buffer(uncompressedSize);

    z_stream stream;
    stream.next_in = const_cast<std::uint8_t *>(data);
    stream.avail_in = compressedSize;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_out = buffer.data();
    stream.avail_out = uncompressedSize;

    int ret = Z_OK;

    ret = inflateInit(&stream);

    switch (ret) {
        case Z_OK: {
            break;
        }
        case Z_MEM_ERROR: {
            std::clog << "inflateInit: Not enough memory." << std::endl;
            break;
        }
        case Z_VERSION_ERROR: {
            std::clog << "inflateInit: zlib version incompatible with assumed version."
                      << std::endl;
            break;
        }
        case Z_STREAM_ERROR: {
            std::clog << "inflateInit: invalid parameters." << std::endl;
            break;
        }
        default: {
            break;
        }
    }

    if (ret != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }

    ret = inflate(&stream, Z_FINISH);

    switch (ret) {
        case Z_OK:
        case Z_STREAM_END: {
            break;
        }
        case Z_MEM_ERROR: {
            std::clog << "inflate: Not enough memory." << std::endl;
            break;
        }
        case Z_DATA_ERROR: {
            std::clog << "inflate: Input data corrupted. Msg: " << std::string(stream.msg)
                      << std::endl;
            break;
        }
        case Z_STREAM_ERROR: {
            std::clog << "inflate: Inconsistent stream structure." << std::endl;
            break;
        }
        default: {
            break;
        }
    }

    if (stream.msg) {
        std::clog << "zlib message: " << std::string(stream.msg) << std::endl;
    }

    if ((ret != Z_OK) && (ret != Z_BUF_ERROR)) {
        throw std::runtime_error("Decompression failed.");
    }

    ret = inflateEnd(&stream);

    switch (ret) {
        case Z_OK: {
            break;
        }
        case Z_STREAM_ERROR: {
            std::clog << "inflateEnd: inconsistent stream state." << std::endl;
            break;
        }
        default: {
            break;
        }
    }

    if (ret != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }

    return (buffer);
}

bool VppEntry::isCompressed() const
{
    return (mCompressed);
}

uint32_t alignToChunk(int n)
{
    if (n == 0) {
        return (n);
    }

    int left = n % CHUNK_SIZE;

    if (left > 0) {
        return (n + (CHUNK_SIZE - left));
    }

    return (n);
}
