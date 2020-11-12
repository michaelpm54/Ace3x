#include "format-readers/vif-mesh.hpp"

#include <cstdio>
#include <cstring>

#include "formats/vif-mesh.hpp"

char **allocate_filenames(const char *buffer, int count, int &totalSize);

VifMesh::~VifMesh()
{
    if (textureNames_0x20_) {
        for (int i = 0; i < numTextures_0x1c_; i++)
            delete[] textureNames_0x20_[i];
        delete[] textureNames_0x20_;
    }
}

void VifMesh::read(const char *buffer)
{
    if (textureNames_0x20_) {
        for (int i = 0; i < numTextures_0x1c_; i++)
            delete[] textureNames_0x20_[i];
        delete[] textureNames_0x20_;
    }
    //        if (vim.rtTextureHandles_0x24)
    //                delete[] vim.rtTextureHandles_0x24;

    puts("==== INIT::HEADER ====");

    // Copy first 0x20 bytes, stop before filenames
    memcpy(&version_0x0_, buffer, 4);
    memcpy(&flags_0x4_, buffer + 4, 4);
    memcpy(&field_0x8_, buffer + 8, 4);
    memcpy(&field_0xc_, buffer + 12, 4);
    memcpy(&field_0x10_, buffer + 16, 4);
    memcpy(&field_0x14_, buffer + 20, 4);
    memcpy(&field_0x18_, buffer + 24, 4);
    memcpy(&numTextures_0x1c_, buffer + 28, 4);

    printf("Version: %04X\n", version_0x0_);
    printf("Flags: %04X\n", flags_0x4_);
    printf("????: %04X\n", field_0x8_);
    printf("????: %04X\n", field_0xc_);
    printf("????: %04X\n", field_0x10_);
    printf("????: %04X\n", field_0x14_);
    printf("????: %04X\n", field_0x18_);

    puts("==== INIT::FILENAMES ====");
    printf("Copying filenames...\n");
    int totalFilenamesSize = 0;
    textureNames_0x20_ = allocate_filenames(buffer, numTextures_0x1c_, totalFilenamesSize);

    fprintf(stdout, "Copied %d filenames of total size 0x%02X (%d)\n", numTextures_0x1c_, totalFilenamesSize, totalFilenamesSize);

    puts("==== INIT::DATA ====");

    printf("Allocating space for texture %d handles...\n", numTextures_0x1c_);
    rtTextureHandles_0x24_ = new uint32_t[numTextures_0x1c_];

    int filenamesStart = 0x20;

    // Seek to 0xFF
    const char *ptr = buffer + totalFilenamesSize + filenamesStart;
    while (((*ptr) & 0xFF) != 0xFF)
        ptr++;

    printf("Buffer 0x%04X data 0x%04X, offset from start = 0x%02X\n", buffer, ptr, reinterpret_cast<intptr_t>(ptr) - reinterpret_cast<intptr_t>(buffer));

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

    /*
                      a
                abdefg0000
        */
    while (c != 0x0) {
        c = *ptr++;
        filenameSize++;
    }
    filenameSize += 2;    // Add the null byte

    /*
                   a  b
                abcdefg0000abc
        */
    paddedSize = filenameSize - 1;

    while ((*ptr & 0xFF) == 0x0) {
        ptr++;
        paddedSize++;
    }

    printf("Filename trimmed size: %d\n", filenameSize);
    printf("Filename padded size: %d\n", paddedSize);

    char **names = new char *[count];
    int offset = 0x20;
    for (int i = 0; i < count; i++) {
        names[i] = new char[filenameSize];
        memcpy(names[i], &buffer[offset], filenameSize);
        printf("Filename %d offset 0x%02X: %s\n", i + 1, offset, names[i]);
        offset += paddedSize;
    }

    totalSize = paddedSize * count;

    return names;
}
