#include <stdio.h>
#include "../../common/glmath.h"
#include "../../common/euler_camera.h"
extern "C" {
#include "../../common/gldl.h"
}

gldl_dt_t *pwnd;
Camera camera;

#define REGION_WIDH 8 //16*16*16 chunks in region
#define CHUNK_WIDTH 16 //16*16*16 voxels in chunk

struct chunk_s {
	vec3 pos;
};

struct region_s {
	vec3int pos;
	chunk_s *p_chunks;
};

region_s region;

#define COORD_TO_OFFSET(width, x, y, z) ((x * width + y) * width + z)

void draw_bbox(vec3 &vmin, vec3 &vmax)
{
	glBegin(GL_LINE_LOOP);
	glVertex3f(vmin.x, vmin.y, vmin.z);
	glVertex3f(vmax.x, vmin.y, vmin.z);
	glVertex3f(vmax.x, vmin.y, vmax.z);
	glVertex3f(vmin.x, vmin.y, vmax.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(vmin.x, vmax.y, vmin.z);
	glVertex3f(vmax.x, vmax.y, vmin.z);
	glVertex3f(vmax.x, vmax.y, vmax.z);
	glVertex3f(vmin.x, vmax.y, vmax.z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(vmin.x, vmin.y, vmin.z);
	glVertex3f(vmin.x, vmax.y, vmin.z);

	glVertex3f(vmax.x, vmin.y, vmin.z);
	glVertex3f(vmax.x, vmax.y, vmin.z);

	glVertex3f(vmax.x, vmin.y, vmax.z);
	glVertex3f(vmax.x, vmax.y, vmax.z);

	glVertex3f(vmin.x, vmin.y, vmax.z);
	glVertex3f(vmin.x, vmax.y, vmax.z);
	glEnd();
}

void draw_bbox_by_center(vec3 &center, float width_half)
{
	vec3 vmin(center.x - width_half, center.y - width_half, center.z - width_half);
	vec3 vmax(center.x + width_half, center.y + width_half, center.z + width_half);
	draw_bbox(vmin, vmax);
}

void draw_point(vec3 &pt)
{
	glBegin(GL_POINTS);
	glVertex3f(pt.x, pt.y, pt.z);
	glEnd();
}

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
	region.pos = {0, 0, 0};
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