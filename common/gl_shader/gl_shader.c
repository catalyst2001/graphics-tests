#include "gl_shader.h"

bool gl_shader_compile(GLuint shader, char *p_dsterr, size_t maxlen, int flags, const char *p_filename, size_t size)
{
	GLenum error;
	memfile_t memfile;
	GLint source_length;
	GLint shader_compile_status = GL_FALSE;
	if (flags & GL_SHADER_SOURCE_FROM_FILE) {
		if (mf_file_to_memory_ex(&memfile, 0, 0, p_filename) != MF_SUCCESS) {
			strcpy_s(p_dsterr, maxlen, "Failed to open file");
			return false;
		}
	}
	else {
		memfile.size = size + 1;
		memfile.pos = 0;
		memfile.p_data = (unsigned char *)calloc(1, memfile.size);
		if (!memfile.p_data)
			return false;

		memcpy(memfile.p_data, p_filename, size);
		printf("----- source -----\n%s\n----------------\n", memfile.p_data);
	}

	source_length = (GLint)strlen(memfile.p_data);
	if (!source_length) {
		strcpy_s(p_dsterr, maxlen, "Empty shader source");
		mf_free_ex(&memfile, 0);
		return false;
	}

	glShaderSource(shader, 1, &memfile.p_data, &source_length);
	glCompileShader(shader);

	// check errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compile_status);
	if (shader_compile_status == GL_FALSE) {
		glGetShaderInfoLog(shader, maxlen, NULL, p_dsterr);
		mf_free_ex(&memfile, 0);
		return false;
	}
	mf_free_ex(&memfile, 0);
	return true;
}
