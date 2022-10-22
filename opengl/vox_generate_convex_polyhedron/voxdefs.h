#pragma once

// voxels flags
#define VF_SOLID (1 << 1)

// one voxel struct
typedef struct voxel_s {
	int data;
} voxel_t;

typedef struct voxel_chunk_s {
	int x_size;
	int y_size;
	int z_size;
	size_t total_size;
	voxel_t *p_voxels;
} voxel_chunk_t;

typedef struct vector3i_s {
	int x, y, z;
} vector3i_t;

inline void vec3i_add(vector3i_t *p_dst, const vector3i_t *p_veca, const vector3i_t *p_vecb)
{
	p_dst->x = p_veca->x + p_vecb->x;
	p_dst->y = p_veca->y + p_vecb->y;
	p_dst->z = p_veca->z + p_vecb->z;
}

inline void vec3i_sub(vector3i_t *p_dst, const vector3i_t *p_veca, const vector3i_t *p_vecb)
{
	p_dst->x = p_veca->x - p_vecb->x;
	p_dst->y = p_veca->y - p_vecb->y;
	p_dst->z = p_veca->z - p_vecb->z;
}