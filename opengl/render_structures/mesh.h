// 
// Render Meshes manager
// 
// Data for meshes is allocated by flag bit priority
// Bit flags can be seen below:
// 
#pragma once
#include "gl_main.h"

// 
// Mesh flags
// 
#define MESH_NORMALS (1 << 1)
#define MESH_UVS (1 << 2)
#define MESH_COLORS (1 << 3)
#define MESH_TANGENT (1 << 4)	 //TODO: MESH_TANGENT flag
#define MESH_BITANGENT (1 << 5)	 //TODO: MESH_BITANGENT flag

#define MESH_DRAWING (1 << 6)

#define MESH_INVALID_INDEX (size_t)(~(size_t)0)

// 
// One mesh data
// 
struct render_mesh_t {
	int flags;	 //mesh flags
	float scale; //mesh scale
	unsigned int uid; //unique data for identity this mesh
	size_t num_of_vertices;
	size_t num_of_indices;
	size_t vao;	 //vertex array object id (if supported VAO), otherwise this filed is not used
	size_t vbo;	 //vertex buffer object id (if supported), otherwise this filed used for store address of vertex buffer
	size_t ibo;	 //index buffer object id (if supported), otherwise this filed used for store address of index buffer
};

// 
// Mesh instance structure
// 
struct render_mesh_instance_t {
	int flags;
	size_t index;          // mesh index in draw list (for optimize)
	render_mesh_t *p_mesh; // address of allocated mesh structure
	vec3 position;		   // position in 3d space
	vec3 rotation;		   // euler rotation
};

// 
// Structure for allocated VAO
// 
struct vao_data_t {
	int flags;	//buffer flags
	size_t vao;	//vertex array object id
};

// 
// Render meshes data
// 
#define RENDER_LEGACY 0
#define RENDER_SUPPORT_VBO (1 << 1)
#define RENDER_SUPPORT_VA0 (1 << 2)

struct render_meshes_t {
	int renderer_supports_flags;
	rutl::dheap<vao_data_t> vaos;							   //allocated vertex array objects
	rutl::dheap<render_mesh_t *> allocated_meshes;			   //allocated meshes
	rutl::dheap<render_mesh_instance_t *> allocated_instances; //allocated mesh instances
	rutl::dheap<render_mesh_instance_t *> draw_list;           //draw list
};

RENDER_STATUS render_meshes_init(render_meshes_t *p_meshes_data, size_t need_meshes, size_t need_instances, size_t need_vaos);
RENDER_STATUS render_meshes_shutdown(render_meshes_t *p_meshes_data);

// 
// Function: render_mesh_allocate
// Purpose: Allocate memory for mesh
// 
// Returns: error code
// 
RENDER_STATUS render_mesh_allocate(render_mesh_t **pp_dst_mesh_object, render_meshes_t *p_render_mesh_manager,
	const char *p_path,
	int flags,
	size_t num_of_vertices,
	size_t num_of_indices,
	const void *p_vertices,
	const void *p_normals,
	const void *p_uvs,
	const void *p_colors,
	const void *p_tangents,
	const void *p_bitangents);

RENDER_STATUS render_mesh_shutdown(render_meshes_t *p_render_mesh_manager, const render_mesh_t *p_mesh_object);

// 
// 
// 
RENDER_STATUS render_draw_add(render_meshes_t *p_render_mesh_manager, render_mesh_instance_t *p_mesh_object);
RENDER_STATUS render_draw_remove(render_meshes_t *p_render_mesh_manager, render_mesh_instance_t *p_mesh_object);

void render_mesh_draw_meshes_12(render_meshes_t *p_render_mesh_manager);
void render_mesh_draw_meshes_13(render_meshes_t *p_render_mesh_manager);
void render_mesh_draw_meshes_40(render_meshes_t *p_render_mesh_manager);