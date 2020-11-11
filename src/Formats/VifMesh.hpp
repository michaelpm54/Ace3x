#ifndef VIFMESH_HPP
#define VIFMESH_HPP

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

class VifMesh {
public:
    VifMesh();
    ~VifMesh();

    void initFromBuffer(const char *buffer);

    //private:
    uint32_t m_version_0x0;
    uint32_t m_flags_0x4;
    uint32_t m_field_0x8;
    uint32_t m_field_0xc;
    uint32_t m_field_0x10;
    uint32_t m_field_0x14;
    uint32_t m_field_0x18;

    uint32_t m_numTextures_0x1c;
    char **m_textureNames_0x20 {nullptr};
    uint32_t *m_rtTextureHandles_0x24;    // 0x1a8

    // --
    uint32_t m_sub0_count;    // 0x1c0
    uint32_t m_sub0_data;     // 0x1c4
    // --

    // --
    uint32_t m_vifBone_count;    // 0x1c8
    uint32_t m_vifBone_data;     // 0x1cc
    // --

    uint32_t m_field_0x4C;    // 0x1d0

    // --
    uint32_t m_sub4_count;    // 0x1d4
    uint32_t m_sub4_data;     // 0x1d8
    // --

    uint32_t m_field_0x58;    // 0x1dc

    // --
    uint32_t m_sub5_count;    // 0x1e0
    uint32_t m_sub5_data;     // 0x1e4
    // --

    // -- ?
    uint32_t **m_data_0x64;    // 0x1e8
    uint32_t m_num_0x68;       // 0x1ec
    // --
};

#endif    // VIFMESH_HPP
