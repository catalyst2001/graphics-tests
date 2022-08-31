#include "mesh.h"

RENDER_STATUS render_meshes_init(render_meshes_t *p_meshes_data, size_t need_meshes, size_t need_instances, size_t need_vaos)
{
	p_meshes_data->renderer_supports_flags = RENDER_LEGACY; //renderer not supports new technologies default

	//check render supports extensions glGenBuffers, glGenVertexArrays
	bool glGenBuffers = true;
	if (glGenBuffers)
		p_meshes_data->renderer_supports_flags |= RENDER_SUPPORT_VBO;

	bool glGenVertexArrays = true;
	if (glGenVertexArrays)
		p_meshes_data->renderer_supports_flags |= RENDER_SUPPORT_VA0;

	// allocate memory
	if (!p_meshes_data->allocated_meshes.alloc(0, need_meshes))
		return RSTATUS_OUT_OF_MEMORY;

	if(!p_meshes_data->allocated_instances.alloc(0, need_instances))
		return RSTATUS_OUT_OF_MEMORY;

	if (!p_meshes_data->vaos.alloc(0, need_vaos))
		return RSTATUS_OUT_OF_MEMORY;

	return RSTATUS_SUCCESS;
}

RENDER_STATUS render_meshes_shutdown(render_meshes_t *p_meshes_data)
{
	p_meshes_data->allocated_meshes.mfree();
	p_meshes_data->allocated_instances.mfree();

#define glDeleteVertexArrays(idx, cnt) //TODO: remove VAO
	for (size_t i = 0; i < p_meshes_data->vaos.get_size(); i++) {
		glDeleteVertexArrays(&p_meshes_data->vaos[i].vao, 1);
	}
	p_meshes_data->vaos.mfree();
	return RSTATUS_SUCCESS;
}

size_t render_alloc_vao_by_flags(render_meshes_t *p_render_mesh_manager, int flags)
{
#define glGenVertexArrays(x, cnt)
	for (size_t i = 0; i < p_render_mesh_manager->vaos.get_size(); i++) {

	}


}

RENDER_STATUS render_mesh_allocate(render_mesh_t **pp_dst_mesh_object, render_meshes_t *p_render_mesh_manager, const char *p_path, int flags, const void *p_data)
{
	render_mesh_t *p_mesh = ALLOC_ONE(render_mesh_t);
	if (!p_mesh)
		return RSTATUS_OUT_OF_MEMORY;

	p_mesh->flags = flags;
	if (p_render_mesh_manager->renderer_supports_flags) {

		// alloc vbo if support
		if (p_render_mesh_manager->renderer_supports_flags & RENDER_SUPPORT_VBO) {

		}

		// alloc vao if support
		if (p_render_mesh_manager->renderer_supports_flags & RENDER_SUPPORT_VA0) {

		}
	} else {
		// legacy version

	}


	*pp_dst_mesh_object = p_mesh;
	return RSTATUS_SUCCESS;
}

RENDER_STATUS render_mesh_shutdown(render_meshes_t *p_render_mesh_manager, const render_mesh_t *p_mesh_object)
{
	return RSTATUS_SUCCESS;
}
