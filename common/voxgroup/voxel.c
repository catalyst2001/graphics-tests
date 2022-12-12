#include "voxel.h"

bool vox_chunk_alloc(voxel_chunk_t *p_vchunk, size_t xsize, size_t ysize, size_t zsize)
{
	p_vchunk->x_size = xsize + 1;
	p_vchunk->y_size = ysize + 1;
	p_vchunk->z_size = zsize + 1;
	p_vchunk->total_size = p_vchunk->x_size * p_vchunk->y_size * p_vchunk->z_size;
	return (bool)(p_vchunk->p_voxels = (voxel_t *)calloc(p_vchunk->total_size, sizeof(voxel_t)));
}

void vox_chunk_free(voxel_chunk_t *p_vchunk)
{
	if (p_vchunk->p_voxels)
		free(p_vchunk->p_voxels);
}

voxel_t *vox_get(voxel_chunk_t *p_vchunk, size_t x, size_t y, size_t z)
{
	size_t offset;
	offset = (x * p_vchunk->z_size + y) * p_vchunk->y_size + x; //TODO: БРЕД!!!
	if (offset >= p_vchunk->total_size)
		return NULL;

	return &p_vchunk->p_voxels[offset];
}

//vector3i_t cube_corners_offsets[] = {
//	{ 0, 0, 0 },
//	{ 1, 0, 0 },
//	{ 0, 0, 1 },
//	{ 1, 0, 1 },
//	{ 0, 1, 0 },
//	{ 1, 1, 0 },
//	{ 0, 1, 1 },
//	{ 1, 1, 1 }
//};
//
//void fill_boundary(bouding_boxi_t *p_dstbound, const vector3i_t *p_srccoord)
//{
//	if (vec3les(&p_dstbound->min, p_srccoord)) {
//		p_dstbound->min = *p_srccoord;
//	}
//	else if (vec3gtr(p_srccoord, &p_dstbound->min)) {
//		p_dstbound->max = *p_srccoord;
//	}
//}
//
///*
// find_group_boundary_points
// найти все граничные точки слоя Y для определенной группы вокселей
//*/
//void find_group_boundary_points(const voxel_chunk_t *p_vox_chunk, int y)
//{
//	voxel_t *p_voxel;
//	bouding_boxi_t boundary_coords;
//	memset(&boundary_coords, 0, sizeof(boundary_coords));
//	for (int x = 0; x < p_vox_chunk->x_size; x++) {
//		for (int z = 0; z < p_vox_chunk->z_size; z++) {
//			if ((p_voxel = vox_get(p_vox_chunk, x, y, z))) {
//
//			}
//		}
//	}
//}

//bool voxel_generate_group(voxel_chunk_t *p_vox_chunk, vox_ushort group)
//{
//	// check voxels for all groups
//	// build a polygonal mesh for the required regions
//	vector3i_t coord;
//	voxel_t *p_voxel;
//	for (coord.y = 0; coord.y < p_vox_chunk->y_size; coord.y++) {
//		for (coord.x = 0; coord.x < p_vox_chunk->x_size; coord.x++) {
//			for (coord.z = 0; coord.z < p_vox_chunk->z_size; coord.z++) {
//				vector3i_t cube_corners[8];
//
//				/*
//					find edges
//					construct cube from left bottom corner
//				*/
//				for (int i = 0; i < 8; i++) {
//					vec3_add(&cube_corners[i], &coord, &cube_corners_offsets[i]);
//					if ((p_voxel = vox_get(p_vox_chunk, cube_corners[i].x, cube_corners[i].y, cube_corners[i].z))) {
//						if (p_voxel->material_group == group) { //was found needed voxel
//
//						}
//					}
//				}
//			}
//		}
//	}
//}

/* V1 */
bool vox_layer_has_all_neighbors(voxel_chunk_t *p_vox_chunk, const vector3i_t *p_coord, int vox_type)
{
	static const vector3i_t neighbor_coords[] = {
		{  0,  0,  0 }, //center
		{ -1,  0,  0 },	//left
		{  1,  0,  0 },	//right
		{  0,  0,  1 },	//forward
		{  0,  0, -1 },	//back
		{ -1,  0,  1 },	//left forward
		{  1,  0, -1 },	//right back
		{  1,  0,  1 },	//right forward
		{ -1,  0, -1 }	//left back
	};

	vector3i_t curr_coord;
	voxel_t *p_current_voxel;
	for (int i = 0; i < sizeof(neighbor_coords) / sizeof(neighbor_coords[0]); i++) {
		vec3i_add(&curr_coord, p_coord, &neighbor_coords[i]);
		if (!(p_current_voxel = vox_get(p_vox_chunk, curr_coord.x, curr_coord.y, curr_coord.z)) || p_current_voxel->data != vox_type) {
			return false;
		}
	}
	return true;
}

bool vox_layer_vox_have_heighbors(voxel_chunk_t *p_vox_chunk, int x, int y, int z, int vox_type)
{
	voxel_t *p_vox;
	if (!(p_vox = vox_get(p_vox_chunk, x + 1, y, z)) || p_vox->data != vox_type)
		return false;
	
	if (!(p_vox = vox_get(p_vox_chunk, x - 1, y, z)) || p_vox->data != vox_type)
		return false;

	if (!(p_vox = vox_get(p_vox_chunk, x, y, z + 1)) || p_vox->data != vox_type)
		return false;

	if (!(p_vox = vox_get(p_vox_chunk, x, y, z - 1)) || p_vox->data != vox_type)
		return false;

	return true;
}

bool vox_layer_vox_have_heighbors_diagonal(voxel_chunk_t *p_vox_chunk, int x, int y, int z, int vox_type)
{
	voxel_t *p_vox;
	if (!(p_vox = vox_get(p_vox_chunk, x + 1, y, z + 1)) || p_vox->data != vox_type)
		return false;

	if (!(p_vox = vox_get(p_vox_chunk, x - 1, y, z + 1)) || p_vox->data != vox_type)
		return false;

	if (!(p_vox = vox_get(p_vox_chunk, x - 1, y, z - 1)) || p_vox->data != vox_type)
		return false;

	if (!(p_vox = vox_get(p_vox_chunk, x + 1, y, z - 1)) || p_vox->data != vox_type)
		return false;

	return true;
}

bool vox_layer_vox_have_heighbors_arround(voxel_chunk_t *p_vox_chunk, int x, int y, int z, int vox_type)
{
	return vox_layer_vox_have_heighbors(p_vox_chunk, x, y, z, vox_type) && vox_layer_vox_have_heighbors_diagonal(p_vox_chunk, x, y, z, vox_type);
}

bool vox_gen_bound_rect(vox_bounding_rect_t *p_dst_bounding_rect, voxel_chunk_t *p_vox_chunk, int y, int vox_type)
{
	int x, z;
	voxel_t *p_vox;

	/* check chunk size */
	if (!p_vox_chunk->x_size || !p_vox_chunk->z_size)
		return false;

	/* set MIN vec to chunk size and MAX vec to 0 */
	p_dst_bounding_rect->min.x = p_vox_chunk->x_size;
	p_dst_bounding_rect->min.z = p_vox_chunk->z_size;
	p_dst_bounding_rect->max.x = 0;
	p_dst_bounding_rect->max.z = 0;
	for (x = 0; x < p_vox_chunk->x_size; x++) {
		for (z = 0; z < p_vox_chunk->z_size; z++) {
			if ((p_vox = vox_get(p_vox_chunk, x, y, z)) && p_vox->data == vox_type) {
				p_dst_bounding_rect->min.x = x;
				p_dst_bounding_rect->min.z = z;
				break;
			}
		}
	}

	for (x = p_vox_chunk->x_size; x > 0; x--) {
		for (z = p_vox_chunk->z_size; z > 0; z--) {
			if ((p_vox = vox_get(p_vox_chunk, x, y, z)) && p_vox->data == vox_type) {
				p_dst_bounding_rect->max.x = x;
				p_dst_bounding_rect->max.z = z;
				break;
			}
		}
	}
}

bool vox_gen_bound_poly(vox_bounding_poly_t *p_dst_layer_poly, voxel_chunk_t *p_vox_chunk, int voxel_type, int y)
{
	vector3i_t vox_start_coord;
	vector3i_t *p_points;
	size_t max_points;
	size_t current_points_count;
	voxel_t *p_current_voxel, *p_next_voxel;

	current_points_count = 0;
	p_dst_layer_poly->number_of_points = 0;
	p_dst_layer_poly->p_points = NULL;
	max_points = (size_t)(p_vox_chunk->x_size * p_vox_chunk->z_size);
	if (!max_points)
		return false;

	max_points /= 2;

	/* allocate temporary memory */
	p_points = (vector3i_t *)malloc(max_points * sizeof(vector3i_t));
	if (!p_points)
		return false;

	/* find begin group voxel */
	p_current_voxel = NULL;
	p_next_voxel = NULL;
	vox_start_coord.y = y;
	for (vox_start_coord.x = 0; vox_start_coord.x < p_vox_chunk->x_size; vox_start_coord.x++)
		for (vox_start_coord.z = 0; vox_start_coord.z < p_vox_chunk->z_size; vox_start_coord.z++)
			if ((p_current_voxel = vox_get(p_vox_chunk, vox_start_coord.x, vox_start_coord.y, vox_start_coord.z)) && p_current_voxel->data == voxel_type)
				break; //first voxel found in layer

	/* check voxel */
	if (!p_current_voxel || p_current_voxel->data != voxel_type)
		return false;

	//TODO: continue!!!

	free(p_points); /* free temp memory */
	return true;
}