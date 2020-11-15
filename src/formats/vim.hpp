/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMATS_VIM_HPP_
#define ACE3X_FORMATS_VIM_HPP_

#include <cstdint>

struct VifMeshSub5 {
    uint32_t data_0x0 {0};
    uint32_t padding[7] {0};
};

struct VifMeshSub4 {
    uint32_t data_0x0 {0};
    uint32_t padding[13] {0};
};

struct VifBone {
    uint32_t boneNameOff {0};
    float x {0};
    float y {0};
    float z {0};
    float a {0};
    float b {0};
    float c {0};
    float padding[21] {0};
};

struct VifMeshSub2 {
    uint32_t off0 {0};
    uint16_t field_0x4 {0};
    uint16_t field_0x6 {0};
};

struct VifMeshSub1 {
    uint16_t field_0x0 {0};
    uint16_t field_0x2 {0};
    uint16_t field_0x4 {0};
    uint16_t field_0x6 {0};
    uint32_t meshIdx {0};
    uint32_t off0 {0};
    uint32_t off1 {0};
    uint32_t field_0x14 {0};
    uint32_t off2 {0};
    uint32_t off3 {0};
    uint32_t off4 {0};
    uint32_t off5 {0};
    uint32_t off6 {0};
};

struct VifMeshSub0 {
    uint16_t field_0x0 {0};
    uint16_t sub1_count {0};
    uint32_t sub2_count {0};
    uint32_t sub1_data {0};    // VifMeshSub1
    uint32_t sub2_data {0};    // VifMeshSub2
};

#endif    // ACE3X_FORMATS_VIM_HPP_
