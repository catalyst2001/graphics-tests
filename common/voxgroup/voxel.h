#ifdef _MSC_VER
#pragma once
#endif

#ifndef __VOXELS_INCLUIDED
#define __VOXELS_INCLUIDED
#include <stddef.h> //size_t
#include "../math.h" //vector3i_t

#define VOX_NOTHING_MACRO(x) //for sizeof in VS intellisense

/* ------------------------ /
  Declarations of common data structures
* ------------------------ */

// voxels flags
#define VF_NULL   (0)
#define VF_SOLID  (1 << 0)
#define VF_AIR    (1 << 1)
#define VF_LIQUID (1 << 2)

typedef unsigned short vox_ushort;
typedef unsigned char vox_byte;
VOX_NOTHING_MACRO(sizeof(vox_ushort));
VOX_NOTHING_MACRO(sizeof(vox_byte));

// one voxel struct
typedef struct voxel_s {
	vox_ushort flags;
	vox_ushort material_group;
	int data;
	unsigned int health;
} voxel_t;
VOX_NOTHING_MACRO(sizeof(voxel_t));

typedef struct voxel_chunk_s {
	int x_size;
	int y_size;
	int z_size;
	size_t total_size;
	voxel_t *p_voxels;
} voxel_chunk_t;
VOX_NOTHING_MACRO(sizeof(voxel_chunk_t));

bool vox_chunk_alloc(voxel_chunk_t *p_vchunk, size_t xsize, size_t ysize, size_t zsize);
void vox_chunk_free(voxel_chunk_t *p_vchunk);

voxel_t *vox_get(voxel_chunk_t *p_vchunk, size_t x, size_t y, size_t z);

/* ------------------------ /
  Declarations of structures and functions for the voxels data storage optimization algorithm

* ------------------------ */

#define VOX_CHUNK_SIGNATURE 0x313041434756 // VGCA01 file signature

enum VOX_SECTIONS {
	VOX_SECTION_GROUPS = 0,
	VOX_SECTION_INDICES,
	VOX_SECTION_VERTICES,
	VOX_MAX_GROUPS
};

// 
// Common file structures
// 
#pragma pack(push, 1)

typedef struct vox_chunk_section_s {
	long offset;
	long size;
} vox_chunk_section_t;
VOX_NOTHING_MACRO(sizeof(vox_chunk_section_t));

typedef struct vox_chunk_hdr_s {
	long long signature;
	vox_chunk_section_t sections[VOX_MAX_GROUPS];
} vox_chunk_hdr_t;
VOX_NOTHING_MACRO(sizeof(vox_chunk_hdr_t));

typedef struct vox_polygon_s {
	long num_of_indices;
} vox_polygon_t;
#pragma pack(pop)

typedef struct vox_group_s {
	int c;
} vox_group_t;


// 
// voxel_generate_group
// 
// generate voxels groups by loaded voxel chunk
// 


/* travel direction */
#define VOX_DIR_FLAG_FORWARD (1 << 0)
#define VOX_DIR_FLAG_BACK (1 << 1)
#define VOX_DIR_FLAG_RIGHT (1 << 2)
#define VOX_DIR_FLAG_LEFT (1 << 3)

#define VOX_DIR_FLAG_TOP (1 << 4)
#define VOX_DIR_FLAG_BOTTOM (1 << 5)

typedef struct vox_xz_s {
	int x, z;
} vox_xz_t;

typedef struct vox_bounding_rect_s {
	vox_xz_t min, max;
} vox_bounding_rect_t;

/* ALGORITHM VERSION 1 */
typedef struct vox_bounding_poly_s {
	size_t number_of_points;
	vector3i_t *p_points;
	vox_bounding_rect_t bounding;
} vox_bounding_poly_t;





#endif