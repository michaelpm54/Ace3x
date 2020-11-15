/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMAT_READERS_VIM_HPP_
#define ACE3X_FORMAT_READERS_VIM_HPP_

#include <cstdint>

class VifMesh {
public:
    ~VifMesh();

    void read(const unsigned char *buffer);

    //private:
    std::uint32_t version_0x0_;
    std::uint32_t flags_0x4_;
    std::uint32_t field_0x8_;
    std::uint32_t field_0xc_;
    std::uint32_t field_0x10_;
    std::uint32_t field_0x14_;
    std::uint32_t field_0x18_;

    std::uint32_t numTextures_0x1c_;
    char **textureNames_0x20_ {nullptr};
    std::uint32_t *rtTextureHandles_0x24_;    // 0x1a8

    // --
    std::uint32_t sub0_count_;    // 0x1c0
    std::uint32_t sub0_data_;     // 0x1c4
    // --

    // --
    std::uint32_t vifBone_count_;    // 0x1c8
    std::uint32_t vifBone_data_;     // 0x1cc
    // --

    std::uint32_t field_0x4C_;    // 0x1d0

    // --
    std::uint32_t sub4_count_;    // 0x1d4
    std::uint32_t sub4_data_;     // 0x1d8
    // --

    std::uint32_t field_0x58_;    // 0x1dc

    // --
    std::uint32_t sub5_count_;    // 0x1e0
    std::uint32_t sub5_data_;     // 0x1e4
    // --

    // -- ?
    std::uint32_t **data_0x64_;    // 0x1e8
    std::uint32_t num_0x68_;       // 0x1ec
    // --
};

#endif    // ACE3X_FORMAT_READERS_VIM_HPP_
