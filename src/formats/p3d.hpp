/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_FORMATS_P3D_HPP_
#define ACE3X_FORMATS_P3D_HPP_

#include "formats/types.hpp"

/* File structure in order:
* Header
* Filenames
* Sub1 data
* Mesh movers data
* Navpoints
* Layers
* Sub2 data
* HTWK data
* HTWK names
* Image names
* Sub3 data
* Other data, includes:
* * texture coordinates
* * vertex colours
* * texture ID's
* * unknown data
*/

struct P3DHeader {
    u32 signature;    // 0x0
    u32 version;      // 0x4
    u32 unk_0x8;

    u32 num_mesh_movers;                                     // 0xC
    u32 ptr_mesh_movers; /* num_mesh_movers * 96 bytes */    // 0x10

    u32 num_sub1_0x14;
    u32 ptr_sub1_0x18;

    u32 num_navpoints;    // 0x1C
    u32 ptr_navpoints;    // 0x20

    u32 num_sub2_0x24;    // Can be zero
    u32 ptr_sub2_0x28;

    u32 num_layers;    // 0x2C
    u32 ptr_layers;    // 0x30

    u32 num_htwk;                                   // Can be zero // 0x34
    u32 ptr_htwk; /* num_htwk * 72 bytes */         // 0x38
    u32 ptr_htwk_names; /* num_htwk * 2 bytes */    // 0x3C

    u32 num_images;    // 0x40
    u32 ptr_images;    // 0x44

    u32 num_sub3_0x48;
    u32 ptr_sub3_0x4C; /* num_sub3_0x48 * 36 bytes */

    f32 x;
    f32 y;
    f32 z;
};

/*
* n = 0xC
* ptr = 0x10
* size = 96 bytes
*/
struct P3DMeshMover {
    f32 vec[3];
    f32 unk_0xC;
    /* repetition 0 */
    u32 unk_0x10[3];
    f32 unk_0x1C;
    /* repetition 1 */
    u32 unk_0x20[3];
    f32 unk_0x2C;
    f32 unk_0x30[6];
    u16 unk_0x48;
    u16 flags;          // 0x4A
    u32 ptr_objinfo;    // 0x4C
    u32 ptr_objtexture;
    u32 ptr_objname;    // 0x54
    u32 unk_0x58;
    u32 unk_0x5C;
};

enum MeshMoverFlags {
    MeshMover_Translucent = 0x2000,
};

/*
* n = 0x14
* ptr = 0x18
* size = 28 bytes
*/
struct P3DObjInfo {
    u32 unk_0x0;
    u32 unk_0x4;
    u32 ptr_0x8;
    u32 ptr_vertices_0xC;
    u32 ptr_indices_0x10;
    u32 ptr_0x14;
    u32 unk_0x18;
};

/*
* n = 0x1C
* ptr = 0x20
* siz = 52 bytes
*/
struct P3DNavpoint {
    u8 unk_0x0[4];
    f32 unk_0x4;
    u8 unk_0x8[12];
    f32 unk_0x14;
    u8 unk_0x18[12];
    f32 unk_0x24;
    f32 x;
    f32 y;
    f32 z;
};

/*
* n = 0x24
* ptr = 0x28
* size = 12 bytes
*/
struct P3DSub2 {
    u32 unk_0x0;
    u16 unk_0x4;
    u16 unk_0x6;
    u32 ptr_0x8;
};

/*
* n = 0x2C
* ptr = 0x30
* size = 12 bytes
*/
struct P3DLayer {
    u32 layer_name;
    u32 num_objects;
    u32 ptr_0xC;
};

/*
* n = 0x34
* ptr = 0x38
* size = 88 bytes
*/
struct P3DHTWK {
    /* 0000014A, 0000014B, 00000152, 00000154 */
    u32 unk_0x0;
    /* FFFF FFFF 0005 0005 */
    u16 unk_0x4;
    /* FFFF FFFF 0004 0004 */
    u16 unk_0x6;
    /* FFFFFFFF FFFFFFFF 00000003 00000002 */
    u32 unk_0x8;
    f32 unk_0xC;
    f32 unk_0x10;
    f32 unk_0x14;
    f32 unk_0x18;
    f32 unk_0x1C;
    f32 unk_0x20;
    f32 unk_0x24;
    f32 unk_0x28;
    f32 unk_0x2C;
    f32 unk_0x30;
    f32 unk_0x34;
    f32 unk_0x38;
    f32 unk_0x3C;
    f32 unk_0x40;
    f32 unk_0x44;
};

/*
* n = 0x34
* ptr = 0x3C
* size = 8 bytes
*/
struct P3DHTWKName {
    u32 ptr;
    u32 unk;
};

/*
* n = 0x48
* ptr = 0x4C
* size = 36 bytes
*/
struct P3DSub3 {
    u32 unk[7];
    u32 ptr_0x1C;
    u32 ptr_0x20;
};

#endif    // ACE3X_FORMATS_P3D_HPP_
