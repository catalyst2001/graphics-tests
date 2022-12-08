#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "../common/glfw/GLFW/glfw3.h"
#include <gl/GLU.h>
#include <stdio.h>
#include <math.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "../common/glfw/glfw3.lib")

#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "../common/stb_truetype.h"

unsigned char ttf_buffer[1 << 20];
unsigned char temp_bitmap[512 * 512];

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
GLuint ftex;

void my_stbtt_initfont(void)
{
	fread(ttf_buffer, 1, 1 << 20, fopen("c:/windows/fonts/segoeui.ttf", "rb"));
	stbtt_BakeFontBitmap(ttf_buffer, 0, 22.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
	// can free ttf_buffer at this point
	glGenTextures(1, &ftex);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void my_stbtt_print(float x, float y, const char *text)
{
	// assume orthographic projection with units = screen pixels, origin at top left
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glBegin(GL_QUADS);
	while (*text) {
		if (*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);//1=opengl & d3d10+,0=d3d9
			glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
			glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
			glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
			glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
		}
		++text;
	}
	glEnd();
}

GLFWwindow* window;
bool has_enable_multisampling = false;

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

const char text[] = "Computer science is the study of computation, automation, and information.[1]\n"
"Computer science spans theoretical disciplines (such as algorithms, theory of computation,\n"
"information theory, and automation) to practical disciplines (including the design and implementation of hardware and software).\n"
"[2][3][4] Computer science is generally considered an area of academic research and distinct from computer programming.[5]\n\n"
"Algorithms and data structures are central to computer science.[6] The theory of computation concerns abstract models\n"
"of computation and general classes of problems that can be solved using them. The fields of cryptography and computer\n"
"security involve studying the means for secure communication and for preventing security vulnerabilities. Computer graphics\n"
"and computational geometry address the generation of images. Programming language theory considers different ways to describe\n"
"computational processes, and database theory concerns the management of repositories of data. Human–computer interaction\n"
"investigates the interfaces through which humans and computers interact, and software engineering focuses on the design\n"
"and principles behind developing software. Areas such as operating systems, networks and embedded systems investigate\n"
"the principles and design behind complex systems. Computer architecture describes the construction of computer components\n"
"and computer-operated equipment. Artificial intelligence and machine learning aim to synthesize goal-orientated processes\n"
"such as problem-solving, decision-making, environmental adaptation, planning and learning found in humans and animals.\n"
"Within artificial intelligence, computer vision aims to understand and process image and video data, while natural\n"
"language processing aims to understand and process textual and linguistic data.";

int main()
{
	printf("Press F1 to switch Multisampling\n\n");
	glfwSetErrorCallback([](int error, const char* description) {
		printf("GLFW error: %d  '%s'\n", error, description);
	});

	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(800, 600, "stb_truetype demo", NULL, NULL);
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

	my_stbtt_initfont();

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		gluOrtho2D(0., width, height, 0.);
		glMatrixMode(GL_MODELVIEW);

		my_stbtt_print(50.f, 50.f, text);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}