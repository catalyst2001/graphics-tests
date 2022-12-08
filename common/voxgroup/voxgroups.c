#include "voxgroups.h"

#include <string.h>

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

void fill_boundary(bouding_boxi_t *p_dstbound, const vector3i_t *p_srccoord)
{
	if (vec3les(&p_dstbound->min, p_srccoord)) {
		p_dstbound->min = *p_srccoord;
	}
	else if (vec3gtr(p_srccoord, &p_dstbound->min)) {
		p_dstbound->max = *p_srccoord;
	}
}

/*
 find_group_boundary_points
 найти все граничные точки слоя Y для определенной группы вокселей
*/
void find_group_boundary_points(const voxel_chunk_t *p_vox_chunk, int y)
{
	voxel_t *p_voxel;
	bouding_boxi_t boundary_coords;
	memset(&boundary_coords, 0, sizeof(boundary_coords));
	for (int x = 0; x < p_vox_chunk->x_size; x++) {
		for (int z = 0; z < p_vox_chunk->z_size; z++) {
			if ((p_voxel = voxel_get(p_vox_chunk, x, y, z))) {

			}
		}
	}
}

bool voxel_generate_group(voxel_chunk_t *p_vox_chunk, vox_ushort group)
{
	// check voxels for all groups
	// build a polygonal mesh for the required regions
	vector3i_t coord;
	voxel_t *p_voxel;
	for (coord.y = 0; coord.y < p_vox_chunk->y_size; coord.y++) {
		for (coord.x = 0; coord.x < p_vox_chunk->x_size; coord.x++) {
			for (coord.z = 0; coord.z < p_vox_chunk->z_size; coord.z++) {
				vector3i_t cube_corners[8];

				/*
					find edges
					construct cube from left bottom corner
				*/
				for (int i = 0; i < 8; i++) {
					vec3_add(&cube_corners[i], &coord, &cube_corners_offsets[i]);
					if ((p_voxel = voxel_get(p_vox_chunk, cube_corners[i].x, cube_corners[i].y, cube_corners[i].z))) {
						if (p_voxel->material_group == group) { //was found needed voxel

						}
					}
				}
			}
		}
	}
}