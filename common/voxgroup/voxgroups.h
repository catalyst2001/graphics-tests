#pragma once
#include "../math.h"
#include "voxdefs.h"



// VGCA01
#define VOX_CHUNK_SIGNATURE 0x313041434756

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

vector3i_t cube_corners_offsets[] = {
	{ 0, 0, 0 },
	{ 1, 0, 0 }, 
	{ 0, 0, 1 }, 
	{ 1, 0, 1 }, 
	{ 0, 1, 0 }, 
	{ 1, 1, 0 }, 
	{ 0, 1, 1 }, 
	{ 1, 1, 1 }
};

typedef struct vox_group_s {
	int c;
} vox_group_t;

// 
// voxel_generate_group
// 
// generate voxels groups by loaded voxel chunk
// 
