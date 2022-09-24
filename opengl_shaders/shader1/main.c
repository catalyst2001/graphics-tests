#include <Windows.h>
#include "../../common/glad/glad.h"
#include "../../common/glfw/GLFW/glfw3.h"
#include <gl/GLU.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "../../common/glfw/glfw3.lib")

#include "../../common/gl_shader/gl_shader.h"

GLFWwindow* window;
bool has_enable_multisampling = true;

GLuint shader_program;
GLuint vertex_shader;
GLuint fragment_shader;

void error_callback(int error, const char* description)
{
	printf("GLFW error: %d  '%s'\n", error, description);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, 1);
			break;

		case GLFW_KEY_F1:
			
			has_enable_multisampling = !has_enable_multisampling;
			if (has_enable_multisampling) {
				printf("Multisampling enabled\n");
				glEnable(GL_MULTISAMPLE);
			}
			else {
				printf("Multisampling disabled\n");
				glDisable(GL_MULTISAMPLE);
			}
			break;
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

int main()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(800, 600, "Shader 1", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return 2;
	}
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader(glfwGetProcAddress)) {
		printf("Failed to init glad\n");
		return 3;
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, width / (double)height, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	char error[512];
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char *p_vs_source =
		"#version 120\n"
		""
		"void main()\n"
		"{"
		"	"
		"	"
		"	"
		"	"
		"	"
		"}";
	if (!gl_shader_compile(vertex_shader, error, sizeof(error), 0, p_vs_source, strlen(p_vs_source))) {
		printf("Vertex shader error:\n------------------------\n%s\n------------------------------\n", error);
		return 1;
	}
	
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *p_fragment_source =
		"#version 120\n"
		""
		"uniform float sin_time;"
		""
		"void main()"
		"{"
		"	float time = abs(sin_time);"
		"	gl_FragColor = vec4(time, time, time, 1.0);"
		"}";
	if (!gl_shader_compile(fragment_shader, error, sizeof(error), 0, p_fragment_source, strlen(p_fragment_source))) {
		printf("Fragment shader error:\n------------------------\n%s\n------------------------------\n", error);
		return 1;
	}

	shader_program = glCreateProgram();
	//glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint sin_time_loc = glGetUniformLocation(shader_program, "sin_time");
	
	GLenum nerror = glGetError();
	if (nerror != GL_NO_ERROR) {
		printf("Error: %d\n", nerror);
		return 1;
	}

	glClearColor(0.20, 0.20, 0.20, 1.0);

	glEnable(GL_TEXTURE_2D);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glTranslatef(0, 0, -5);

		glUseProgram(shader_program);
		glUniform1f(sin_time_loc, sinf(glfwGetTime()));

		glBegin(GL_QUADS);
		glVertex3f(-1, -1, 0);
		glVertex3f(-1, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, -1, 0);
		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glUseProgram(0);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shader_program);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}