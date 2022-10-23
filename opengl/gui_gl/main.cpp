#include <Windows.h>
#include "../../common/glfw/GLFW/glfw3.h"
#include <gl/GLU.h>
#include <stdio.h>
#include <math.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "../../common/glfw/glfw3.lib")

GLFWwindow* window;
bool has_enable_multisampling = false;

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#include "rgui.h"

rgui_root_dispatcher g_gui;
rgui_root_dispatcher *p_gui = &g_gui;

int main()
{
	printf("Press F1 to switch Multisampling\n\n");
	glfwSetErrorCallback([](int error, const char* description) {
		printf("GLFW error: %d  '%s'\n", error, description);
	});

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(800, 600, "OpenGL gui", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return 2;
	}

	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
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
		p_gui->keydown(key, (rbutton_state)action);
	});
	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, width / (double)height, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_MULTISAMPLE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		gluOrtho2D(0., width, 0., height);
		glMatrixMode(GL_MODELVIEW);

		p_gui->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}