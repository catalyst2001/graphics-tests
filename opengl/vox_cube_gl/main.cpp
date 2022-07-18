#include <stdio.h>
#include "../../common/glmath.h"
#include "../../common/euler_camera.h"
extern "C" {
#include "../../common/gldl.h"
}
Camera camera;

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
	region.pos = { 0, 0, 0 };
	region.p_chunks = (chunk_s *)calloc(REGION_WIDH*REGION_WIDH*REGION_WIDH, sizeof(chunk_s));
	for (int y = 0; y < REGION_WIDH; y++) {
		for (int x = 0; x < REGION_WIDH; x++) {
			for (int z = 0; z < REGION_WIDH; z++) {
				region.p_chunks[COORD_TO_OFFSET(REGION_WIDH, x, y, z)].pos(x * REGION_WIDH, y * REGION_WIDH, z * REGION_WIDH);
			}
		}
	}

	glPointSize(2.f);
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera.Look(pwnd->GetWindow());

		glPushAttrib(GL_CURRENT_BIT);

		glColor4ub(255, 255, 255, 40);
		pwnd->Draw3DGird(0, 0, 0, 100, 5);

		glColor4ub(0, 255, 0, 255);
		chunk_s *p_chunk;
		int half_region_width = REGION_WIDH;
		for (int y = region.pos.y - half_region_width; y < half_region_width; y++) {
			for (int x = region.pos.x - half_region_width; x < half_region_width; x++) {
				for (int z = region.pos.z - half_region_width; z < half_region_width; z++) {
					p_chunk = &region.p_chunks[COORD_TO_OFFSET(REGION_WIDH, (x % REGION_WIDH), (y % REGION_WIDH), (z % REGION_WIDH))];
					draw_point(p_chunk->pos);
					draw_bbox_by_center(p_chunk->pos, CHUNK_WIDTH / 2);
				}
			}
		}

		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	return 0;
}