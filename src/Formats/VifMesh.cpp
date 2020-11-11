#include "VifMesh.hpp"

#include <cstdio>
#include <cstring>

char **allocate_filenames(const char *buffer, int count, int &totalSize);

VifMesh::VifMesh()
{
}

VifMesh::~VifMesh()
{
    if (m_textureNames_0x20) {
        for (int i = 0; i < m_numTextures_0x1c; i++)
            delete[] m_textureNames_0x20[i];
        delete[] m_textureNames_0x20;
    }
}

void VifMesh::initFromBuffer(const char *buffer)
{
    if (m_textureNames_0x20) {
        for (int i = 0; i < m_numTextures_0x1c; i++)
            delete[] m_textureNames_0x20[i];
        delete[] m_textureNames_0x20;
    }
    //        if (vim.rtTextureHandles_0x24)
    //                delete[] vim.rtTextureHandles_0x24;

    puts("==== INIT::HEADER ====");

    // Copy first 0x20 bytes, stop before filenames
    memcpy(&m_version_0x0, buffer, 4);
    memcpy(&m_flags_0x4, buffer + 4, 4);
    memcpy(&m_field_0x8, buffer + 8, 4);
    memcpy(&m_field_0xc, buffer + 12, 4);
    memcpy(&m_field_0x10, buffer + 16, 4);
    memcpy(&m_field_0x14, buffer + 20, 4);
    memcpy(&m_field_0x18, buffer + 24, 4);
    memcpy(&m_numTextures_0x1c, buffer + 28, 4);

    printf("Version: %04X\n", m_version_0x0);
    printf("Flags: %04X\n", m_flags_0x4);
    printf("????: %04X\n", m_field_0x8);
    printf("????: %04X\n", m_field_0xc);
    printf("????: %04X\n", m_field_0x10);
    printf("????: %04X\n", m_field_0x14);
    printf("????: %04X\n", m_field_0x18);

    puts("==== INIT::FILENAMES ====");
    printf("Copying filenames...\n");
    int totalFilenamesSize = 0;
    m_textureNames_0x20 = allocate_filenames(buffer, m_numTextures_0x1c, totalFilenamesSize);

    fprintf(stdout, "Copied %d filenames of total size 0x%02X (%d)\n", m_numTextures_0x1c, totalFilenamesSize, totalFilenamesSize);

    puts("==== INIT::DATA ====");

    printf("Allocating space for texture %d handles...\n", m_numTextures_0x1c);
    m_rtTextureHandles_0x24 = new uint32_t[m_numTextures_0x1c];

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

    memcpy(&m_sub0_count, ptr, 4);
    memcpy(&m_sub0_data, ptr + 4, 4);
    memcpy(&m_vifBone_count, ptr + 8, 4);
    memcpy(&m_vifBone_data, ptr + 12, 4);
    memcpy(&m_field_0x4C, ptr + 16, 4);
    memcpy(&m_sub4_count, ptr + 20, 4);
    memcpy(&m_sub4_data, ptr + 24, 4);
    memcpy(&m_field_0x58, ptr + 28, 4);
    memcpy(&m_sub5_count, ptr + 32, 4);
    memcpy(&m_sub5_data, ptr + 36, 4);
    memcpy(&m_data_0x64, ptr + 40, 4);
    memcpy(&m_num_0x68, ptr + 44, 4);

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
