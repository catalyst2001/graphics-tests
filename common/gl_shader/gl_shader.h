#pragma once
#include <stdbool.h>
#include "../glad/glad.h"
#include "../memfile.h"

#define GL_SHADER_SOURCE_FROM_FILE (1 << 1)

/* gl_shader_compile()
*  
*  Compiles the specified shader from a file or from memory, depending on the specified options.
*  If the flags parameter has the GL_SHADER_SOURCE_FROM_FILE bit, the p_filename parameter expects a path to a file, the size parameter is ignored.
*  Otherwise, p_filename will be interpreted as a pointer to the beginning of the memory with the text of the shader's source code, and the size parameter will report the length of the buffer where the text of the shader's source code is stored.
* 
*  Return value: true if the shader was compiled without errors, otherwise the function returns false and the error information is copied to the p_dsterr buffer
*/ 
bool gl_shader_compile(GLuint shader, char *p_dsterr, size_t maxlen, int flags, const char *p_filename, size_t size);