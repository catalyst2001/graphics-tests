#include "voxgroups.h"

bool voxel_generate_group(voxel_chunk_t *p_vox_chunk)
{
	// check voxels for all groups
	// build a polygonal mesh for the required regions
	vector3i_t coord;
	voxel_t *p_voxel, *p_neighbor_voxel;
	for (int group = 0; group < 10; group++) {
		for (coord.y = 0; coord.y < p_vox_chunk->y_size; coord.y++) {
			for (coord.x = 0; coord.x < p_vox_chunk->x_size; coord.x++) {
				for (coord.z = 0; coord.z < p_vox_chunk->z_size; coord.z++) {
					if ((p_voxel = voxel_get(p_vox_chunk, coord.x, coord.y, coord.z))) {

						/* construct cube from left bottom corner */
						vector3i_t cube_corners[8];
						for (int i = 0; i < 8; i++)
							vec3_add(&cube_corners[i], &coord, &cube_corners_offsets[i]);
						
						/* find edges */
						for (int i = 0; i < 8; i++) {
							if ((p_neighbor_voxel = voxel_get(p_vox_chunk, cube_corners[i].x, cube_corners[i].y, cube_corners[i].z))) {
								if (p_neighbor_voxel->material_group == group) { //was found needed voxel

								}
							}
						}
					}
				}
			}
		}
	}
}