#include <Windows.h>
#include "../../common/glfw/GLFW/glfw3.h"
#include <gl/GLU.h>
#include <stdio.h>
#include <math.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "../../common/glfw/glfw3.lib")

#include "../../common/glmath.h"
#include "../../common/euler_camera.h"
#include "../../common/SimplexNoise.h"

#include "voxel.h"

bool wireframe_mode = true;
GLFWwindow* window;
Camera camera(true);
HWND wnd;

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
vec3 VertexInterp(double isolevel, vec3 p1, vec3 p2, double valp1, double valp2)
{
	double mu;
	vec3 p;
	if (fabs(isolevel - valp1) < 0.00001)
		return(p1);

	if (fabs(isolevel - valp2) < 0.00001)
		return(p2);

	if (fabs(valp1 - valp2) < 0.00001)
		return(p1);

	mu = (isolevel - valp1) / (valp2 - valp1);
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);
	return (p);
}

CChunk chunk;

int main()
{
	glfwSetErrorCallback([](int error, const char* description) {
		printf("GLFW error: %d  '%s'\n", error, description);
	});

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(800, 600, "Multisampling test", NULL, NULL);
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

	wnd = FindWindowA(0, "Multisampling test");
	if (!wnd) {
		printf("Failed to find window!\n");
	}

	glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, 1);
				break;

			case GLFW_KEY_F1:
				camera.updatemouse = !camera.updatemouse;
				break;

			case GLFW_KEY_F2:
				wireframe_mode = !wireframe_mode;
				glPolygonMode(GL_FRONT_AND_BACK, wireframe_mode ? GL_LINE : GL_FILL);
				break;
			}
		}
	});
	glfwMakeContextCurrent(window);

	glPolygonMode(GL_FRONT_AND_BACK, wireframe_mode ? GL_LINE : GL_FILL);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, width / (double)height, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CVoxel *p_vox;
	SimplexNoise snoise;
	chunk.Init(vec3int(0, 0, 0), 16, 250);
	chunk.m_nDDBounds = true;

	float nfrequency = 0.003f; //noise frequency
	float namplitude = 15.2f; //noise amplitude
	for (int y = 0; y <= chunk.GetChunkHeight(); y++) {
		for (int x = 0; x <= chunk.GetChunkWidth(); x++) {
			for (int z = 0; z <= chunk.GetChunkWidth(); z++) {
				float max_height = (float)chunk.GetChunkHeight();
				float pn1v = snoise.noise((float)x * nfrequency, (float)z * nfrequency) * namplitude; //primary noise value
				int voxel_height = floor(max_height - pn1v);
				if ((p_vox = chunk.VoxelAt(x, y, z)) && (y < voxel_height)) {
					p_vox->SetFlag(VOXEL_FLAG_SOLID);
				}
			}
		}
	}
	chunk.RebuildMesh();

	glEnableClientState(GL_VERTEX_ARRAY);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera.Look(wnd);

		chunk.DrawMesh();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}