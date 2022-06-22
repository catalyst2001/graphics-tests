#include <stdio.h>
#include "../../common/glmath.h"
#include "../../common/euler_camera.h"
extern "C" {
#include "../../common/gldl.h"
}

gldl_dt_t *pwnd;
Camera camera;

#define CHUNK_WIDTH 16 //16*16*16 voxels in chunk
#define REGION_WIDH 16 //16*16*16 chunks in region

struct chunk_s {
	vec3 pos;
};

struct region_s {
	chunk_s *p_chunks;
};

region_s region;

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

	if (!(pwnd = gldl_initialize(-1, -1, 800, 600, 24, 32, "Camera move anim record", &events))) {
		printf("Failed to create window!\n");
		return 1;
	}

	vec3 result, vec1 = { 10, 10, 0 }, vec2 = { 0, 2, 10 };
	glViewport(0, 0, pwnd->GetWindowWidth(), pwnd->GetWindowHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, pwnd->GetWindowWidth() / (double)pwnd->GetWindowHeight(), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// --- init chunks ---
	region.p_chunks = (chunk_s *)calloc(REGION_WIDH*REGION_WIDH*REGION_WIDH, sizeof(chunk_s));
	for (int y = 0; y < REGION_WIDH; y++) {
		for (int x = 0; x < REGION_WIDH; x++) {
			for (int z = 0; z < REGION_WIDH; z++) {
				region.p_chunks[(x * REGION_WIDH + y) * REGION_WIDH + z];
			}
		}
	}

	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera.Look(pwnd->GetWindow());

		glPushAttrib(GL_CURRENT_BIT);

		glColor4ub(255, 255, 255, 40);
		pwnd->Draw3DGird(0, 0, 0, 100, 5);

		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	return 0;
}