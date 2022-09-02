#include "mesh.h"

#define glGenBuffers(n, p)
#define glBindVertexArray(a)
#define glGenVertexArrays(n, arrays)
#define glBindBuffer(target, ibuffer)
#define glGenVertexArrays(x, cnt)
#define glDeleteVertexArrays(idx, cnt)
#define glBufferData(type, data_size_in_bytes, n, access)

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
	for (size_t i = 0; i < p_meshes_data->vaos.get_size(); i++) {
		glDeleteVertexArrays(&p_meshes_data->vaos[i].vao, 1);
	}
	p_meshes_data->vaos.mfree();
	return RSTATUS_SUCCESS;
}

size_t render_alloc_vao_by_flags(render_meshes_t *p_render_mesh_manager, int flags)
{
	// find exists vao's
	vao_data_t *p_vaodata;
	for (size_t i = 0; i < p_render_mesh_manager->vaos.get_size(); i++)
		if ((p_vaodata = p_render_mesh_manager->vaos.get_at_ptr(i)))
			if (p_vaodata->flags == flags)
				return p_vaodata->vao;

	// if not found
	// allocate new vao
	vao_data_t new_vao;
	new_vao.flags = flags;
	glGenVertexArrays(&new_vao.vao, 1);
}

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
	const void *p_bitangents)
{
	render_mesh_t *p_mesh = ALLOC_ONE(render_mesh_t);
	if (!p_mesh)
		return RSTATUS_OUT_OF_MEMORY;

	p_mesh->flags = flags;
	p_mesh->num_of_vertices = num_of_vertices;
	p_mesh->num_of_indices = num_of_indices;
	if (p_render_mesh_manager->renderer_supports_flags) {
		// alloc vao if support
		if (p_render_mesh_manager->renderer_supports_flags & RENDER_SUPPORT_VA0) {
			glGenVertexArrays(1, p_mesh->vao);

		}

		// alloc vbo if support
		if (p_render_mesh_manager->renderer_supports_flags & RENDER_SUPPORT_VBO) {
			glGenBuffers(1, &p_mesh->vbo);
			glGenBuffers(1, &p_mesh->ibo);

			glBindVertexArray(p_mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, p_mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, p_mesh->num_of_vertices * sizeof(vec3), )
		}
	} else {

		// legacy opengl version

	}
	*pp_dst_mesh_object = p_mesh;
	return RSTATUS_SUCCESS;
}

RENDER_STATUS render_mesh_shutdown(render_meshes_t *p_render_mesh_manager, const render_mesh_t *p_mesh_object)
{
	return RSTATUS_SUCCESS;
}

RENDER_STATUS render_draw_add(render_meshes_t *p_render_mesh_manager, render_mesh_instance_t *p_mesh_object)
{
	p_mesh_object->flags |= MESH_DRAWING; // set drawing flag
	if (!p_render_mesh_manager->draw_list.push((render_mesh_instance_t *)p_mesh_object)) // add instance to draw list
		return RSTATUS_OUT_OF_MEMORY;

	p_mesh_object->index = p_render_mesh_manager->draw_list.get_size(); //save mesh instance index in array
	return RSTATUS_SUCCESS;
}

RENDER_STATUS render_draw_remove(render_meshes_t *p_render_mesh_manager, render_mesh_instance_t *p_mesh_object)
{
	// if mesh instance
	if (p_mesh_object->flags & MESH_DRAWING) {
		p_mesh_object->flags &= ~MESH_DRAWING; //remove drawing flag
		if (!p_render_mesh_manager->draw_list.remove_fast(p_mesh_object->index))
			return RSTATUS_NOT_FOUND;

		p_mesh_object->index = MESH_INVALID_INDEX; //set size_t max (free index)
		return RSTATUS_SUCCESS;
	}
	return RSTATUS_NOT_FOUND; //mesh not found in draw list
}

void render_mesh_draw_meshes_40(render_meshes_t *p_render_mesh_manager)
{
	render_mesh_t *p_mesh;
	render_mesh_instance_t *p_mesh_instance;
	for (size_t i = 0; i < p_render_mesh_manager->draw_list.get_size(); i++) {
		p_mesh_instance = p_render_mesh_manager->draw_list.get_at_ref(i);
		p_mesh = p_mesh_instance->p_mesh;
		assert(p_mesh_instance);
		assert(p_mesh);

		glPushMatrix();
		glTranslatef(p_mesh_instance->position.x, p_mesh_instance->position.y, p_mesh_instance->position.z);
#define glBindVertexArray(vao)
#define glDrawElements(ptype, idxcnt, type, idxptr)
		glBindVertexArray(p_mesh->vao);
		glDrawElements(GL_TRIANGLES, p_mesh->num_of_indices, GL_UNSIGNED_INT, NULL);
		glPopMatrix();
	}
}