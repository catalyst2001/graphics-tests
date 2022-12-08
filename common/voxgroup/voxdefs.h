#pragma once
#define VOX_NOTHING_MACRO(x)

// voxels flags
#define VF_SOLID (1 << 1)

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