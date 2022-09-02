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

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}