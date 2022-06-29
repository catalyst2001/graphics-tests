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

int main()
{
	printf("Press F1 to switch Multisampling\n\n");
	glfwSetErrorCallback([](int error, const char* description) {
		printf("GLFW error: %d  '%s'\n", error, description);
	});

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(800, 600, "Multisampling test", NULL, NULL);
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

		int center[2];
		center[0] = width / 2;
		center[1] = height / 2;

		float radius = 250.f;
		float triangleAmount = 100;
		float twicePi = 3.14 * 2;
		glBegin(GL_LINES);
		for (float i = 0; i <= triangleAmount; i++) {
			glVertex2i(center[0], center[1]);
			glVertex2f(
				center[0] + (radius * cos(i * twicePi / triangleAmount)),
				center[1] + (radius * sin(i * twicePi / triangleAmount))
			);
		}
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex2i(
			center[0] + (radius * cos(0.f * twicePi / triangleAmount)),
			center[1] + (radius * sin(0.f * twicePi / triangleAmount))
		);
		for (float i = 1.f; i <= triangleAmount; i++) {
			glVertex2f(
				center[0] + (radius * cos(i * twicePi / triangleAmount)),
				center[1] + (radius * sin(i * twicePi / triangleAmount))
			);
		}
		glEnd();

		center[0] = 100;
		center[1] = 100;
		radius = 20.f;
		glBegin(GL_POLYGON);
		glVertex2i(
			center[0] + (radius * cos(0.f * twicePi / triangleAmount)),
			center[1] + (radius * sin(0.f * twicePi / triangleAmount))
		);
		for (float i = 1.f; i <= triangleAmount; i++) {
			glVertex2f(
				center[0] + (radius * cos(i * twicePi / triangleAmount)),
				center[1] + (radius * sin(i * twicePi / triangleAmount))
			);
		}
		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}