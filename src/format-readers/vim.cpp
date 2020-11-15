/* SPDX-License-Identifier: GPLv3-or-later */

#include "format-readers/vim.hpp"

#include <spdlog/spdlog.h>

#include <cassert>
#include <cstdio>
#include <cstring>

#include "formats/vim.hpp"

char **allocate_filenames(const char *buffer, int count, int &totalSize);

VifMesh::~VifMesh()
{
    if (textureNames_0x20_) {
        for (auto i = 0u; i < numTextures_0x1c_; i++)
            delete[] textureNames_0x20_[i];
        delete[] textureNames_0x20_;
    }
}

void VifMesh::read(const unsigned char *buffer)
{
    assert(buffer);

    if (textureNames_0x20_) {
        for (auto i = 0u; i < numTextures_0x1c_; i++)
            delete[] textureNames_0x20_[i];
        delete[] textureNames_0x20_;
    }
    //        if (vim.rtTextureHandles_0x24)
    //                delete[] vim.rtTextureHandles_0x24;

    spdlog::debug("==== VIM::HEADER ====");

    // Copy first 0x20 bytes, stop before filenames
    memcpy(&version_0x0_, buffer, 4);
    memcpy(&flags_0x4_, buffer + 4, 4);
    memcpy(&field_0x8_, buffer + 8, 4);
    memcpy(&field_0xc_, buffer + 12, 4);
    memcpy(&field_0x10_, buffer + 16, 4);
    memcpy(&field_0x14_, buffer + 20, 4);
    memcpy(&field_0x18_, buffer + 24, 4);
    memcpy(&numTextures_0x1c_, buffer + 28, 4);

    spdlog::debug("VIM: Version: %04X", version_0x0_);
    spdlog::debug("VIM: Flags: %04X", flags_0x4_);

    assert(version_0x0_ == 0xB);

    spdlog::debug("==== VIM::FILENAMES ====");
    spdlog::debug("VIM: Copying filenames...");

    int totalFilenamesSize = 0;
    textureNames_0x20_ = allocate_filenames(reinterpret_cast<const char *>(buffer), numTextures_0x1c_, totalFilenamesSize);

    spdlog::debug("VIM: Copied {} filenames of total size {:02x} ({})", numTextures_0x1c_, totalFilenamesSize, totalFilenamesSize);

    spdlog::debug("==== VIM::DATA ====");

    spdlog::debug("Allocating space for texture {} handles...", numTextures_0x1c_);
    rtTextureHandles_0x24_ = new uint32_t[numTextures_0x1c_];

    int filenamesStart = 0x20;

    // Seek to 0xFF
    auto *ptr = buffer + totalFilenamesSize + filenamesStart;
    while (((*ptr) & 0xFF) != 0xFF)
        ptr++;

    spdlog::debug("Buffer {:p} data {:p}, offset from start = {:td}", (void *)buffer, (void *)ptr, reinterpret_cast<intptr_t>(ptr) - reinterpret_cast<intptr_t>(buffer));

    // Seek past 0xFF's
    while (((*ptr) & 0xFF) == 0xFF)
        ptr++;
    ptr += 4;

    memcpy(&sub0_count_, ptr, 4);
    memcpy(&sub0_data_, ptr + 4, 4);
    memcpy(&vifBone_count_, ptr + 8, 4);
    memcpy(&vifBone_data_, ptr + 12, 4);
    memcpy(&field_0x4C_, ptr + 16, 4);
    memcpy(&sub4_count_, ptr + 20, 4);
    memcpy(&sub4_data_, ptr + 24, 4);
    memcpy(&field_0x58_, ptr + 28, 4);
    memcpy(&sub5_count_, ptr + 32, 4);
    memcpy(&sub5_data_, ptr + 36, 4);
    memcpy(&data_0x64_, ptr + 40, 4);
    memcpy(&num_0x68_, ptr + 44, 4);

    fflush(stdout);
}

char **allocate_filenames(const char *buffer, int count, int &totalSize)
{
    int filenameSize = 0;
    int paddedSize = 0;

    const char *ptr = &buffer[0x20];
    char c = *ptr++;

    while (c != 0x0) {
        c = *ptr++;
        filenameSize++;
    }
    filenameSize += 2;    // Add the null byte

    paddedSize = filenameSize - 1;

    while ((*ptr & 0xFF) == 0x0) {
        ptr++;
        paddedSize++;
    }

    spdlog::debug("VIM: Filename trimmed size: {}", filenameSize);
    spdlog::debug("VIM: Filename padded size: {}", paddedSize);

    char **names = new char *[count];
    int offset = 0x20;
    for (int i = 0; i < count; i++) {
        names[i] = new char[filenameSize];
        memcpy(names[i], &buffer[offset], filenameSize);
        spdlog::debug("VIM: Filename {} offset {:02x}: {}", i + 1, offset, names[i]);
        offset += paddedSize;
    }

    totalSize = paddedSize * count;

    return names;
}
