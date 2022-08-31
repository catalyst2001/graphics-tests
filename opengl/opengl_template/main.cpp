#include <Windows.h>
#include "glad.h"
#include <gl/GLU.h>
#include "GLFW/glfw3.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glfw3.lib")

#define APPTITLE "Title" //заголовок приложения
#define WIDTH 800 //ширина окна
#define HEIGHT 600 //высота окна

#include <stdio.h>

GLFWwindow* window;

// 3d вектор
struct vec3 {
	float x, y, z;
};

//RGB цвет
struct color3 {
	unsigned char r, g, b;
};

//структура вершины
struct vertex {
	vec3 vtx;
	color3 color;
};

//модель кубика
vertex cube_model[] = {
	//левая боковая стенка
	{ { -1,  1,  1 }, { 0, 0, 255 } },
	{ { -1, -1,  1 }, { 255, 0, 0 } },
	{ { -1, -1, -1 }, { 0, 255, 0 } },
	{ { -1,  1, -1 }, { 128, 128, 128 } },

	//передняя боковая стенка
	{ { -1,  1, -1 }, { 128, 128, 128 } },
	{ { -1, -1, -1 }, { 0, 255, 0 } },
	{ {  1, -1, -1 }, { 255, 255, 255 } },
	{ {  1,  1, -1 }, { 255, 255, 255 } },

	//правая боковая стенка
	{ {  1,  1, -1 }, { 255, 255, 255 } },
	{ {  1, -1,  1 }, { 255, 255, 255 } },
	{ {  1, -1,  1 }, { 255, 255, 255 } },
	{ {  1,  1,  1 }, { 255, 255, 255 } },

	//задняя боковая стенка
	{ {  1,  1,  1 }, { 255, 255, 255 } },
	{ {  1, -1,  1 }, { 255, 255, 255 } },
	{ { -1, -1,  1 }, { 255, 0, 0 } },
	{ { -1,  1,  1 }, { 0, 0, 255 } },

	//низ
	{ { -1, -1, -1 }, { 0, 255, 0 } },
	{ {  1, -1, -1 }, { 255, 255, 255 } },
	{ {  1, -1,  1 }, { 255, 255, 255 } },
	{ { -1, -1,  1 }, { 255, 0, 0 } },

	//верх
	{ { -1,  1, -1 }, { 128, 128, 128 } },
	{ { -1,  1,  1 }, { 0, 0, 255 } },
	{ {  1,  1,  1 }, { 255, 255, 255 } },
	{ {  1,  1, -1 }, { 255, 255, 255 } }
};

int main()
{
	glfwSetErrorCallback([](int error, const char* description) {
		printf("GLFW error: %d  '%s'\n", error, description);
	});

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(WIDTH, HEIGHT, APPTITLE, NULL, NULL);
	if (!window) {
		glfwTerminate();
		return 2;
	}

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.f, width / (double)height, 1.0, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	});

	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, 1);
				break;
			}
		}
	});
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to init glad!\n");
		return 1;
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, width / (double)height, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float angle = 0.f;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glTranslatef(0.f, 0.f, -10.f);
		glRotatef(angle, 1, 0, 0);
		glRotatef(angle, 0, 1, 0);
		angle += 0.01;

		glVertexPointer(3, GL_FLOAT, sizeof(vertex), ((char *)cube_model + offsetof(vertex, vtx)));
		glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(vertex), ((char *)cube_model + offsetof(vertex, color)));
		glDrawArrays(GL_QUADS, 0, 24);

		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}