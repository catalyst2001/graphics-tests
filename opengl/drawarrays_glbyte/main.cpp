#include <stdio.h>
#include "../../common/glmath.h"
#include "../../common/euler_camera.h"
extern "C" {
#include "../../common/gldl.h"
#include "../../common/texture.h"
}

gldl_dt_t *pwnd;
Texture texture;

template<typename _T>
struct vector2
{
	union {
		struct { _T x, y; };
		struct { _T u, v; };
	};
};

int main()
{
	gldl_events_dt_t events;
	memset(&events, NULL, sizeof(events));
	events.p_keydown_input_event = [](int keycode, int state) {
		if (state == STATE_PRESSED) {
			switch (keycode) {
			case 27:
				PostQuitMessage(0);
				break;
			}
		}
	};

	if (!(pwnd = gldl_initialize(-1, -1, 800, 600, 24, 32, "GL_BYTE for normals and VU's test", &events))) {
		printf("Failed to create window!\n");
		return 1;
	}

	glViewport(0, 0, pwnd->GetWindowWidth(), pwnd->GetWindowHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, pwnd->GetWindowWidth() / (double)pwnd->GetWindowHeight(), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (!LoadTexture("../../data/uv.bmp", &texture))
		printf("Failed to load texture!\n");

	glBindTexture(GL_TEXTURE_2D, texture.texID);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, pwnd->GetWindowWidth(), pwnd->GetWindowHeight());
		gluOrtho2D(0., pwnd->GetWindowWidth(), 0., pwnd->GetWindowHeight());
		glMatrixMode(GL_MODELVIEW);

		int vertices[][2] = {
			{100,             100},
			{100 + 500,       100},
			{100 + 500, 100 + 500},
			{100,       100 + 500}
		};

		vector2<char> uvs[] = {
			{0,   1}, 
			{1,    1},
			{1,   0},
			{0,  0}
		};
		glVertexPointer(2, GL_INT, 0, vertices);
		glTexCoordPointer(2, GL_BYTE, 0, uvs);
		glDrawArrays(GL_QUADS, 0, 4);
	}
	gldl_shutdown(pwnd);
	return 0;
}