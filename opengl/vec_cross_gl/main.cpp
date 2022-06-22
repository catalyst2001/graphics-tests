#include <stdio.h>
#include "../../common/glmath.h"
#include "../../common/euler_camera.h"
extern "C" {
#include "../../common/gldl.h"
}

gldl_dt_t *pwnd;
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

	vec3 result, vec1 = { 10, 0, 0 }, vec2 = { 0, 0, 10 };
	glViewport(0, 0, pwnd->GetWindowWidth(), pwnd->GetWindowHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, pwnd->GetWindowWidth() / (double)pwnd->GetWindowHeight(), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera.Look(pwnd->GetWindow());

		glPushAttrib(GL_CURRENT_BIT);

		//pwnd->Draw3DAxises(2.f, 2.f);
		glColor4ub(255, 255, 255, 40);
		pwnd->Draw3DGird(0, 0, 0, 100, 5);

		result = cross(vec1, vec2);
		result = -result;

		//vec1 = normalize(vec1);
		//vec2 = normalize(vec2);
		//float d = dot(vec1, vec2);
		//float deg = acosf(d);
		//if (!(GetTickCount() % 100))
		//	printf("Degress: %f  Dot: %f\n", deg * RTOD, d);

		// --- camera dir
		vec3 center(1.f, 1.f, 1.f);
		vec3 dir = normalize(camera.Dir - camera.Position);
		float deg = acosf(dot(dir, center)) * RTOD;
		printf("Degress of camera: %f  ( %f %f %f )\n", deg, camera.Dir.x, camera.Dir.y, camera.Dir.z);
		
		glLineWidth(2.f);
		glColor4ub(0, 255, 0, 255);

		glBegin(GL_LINES);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(vec1.x, vec1.y, vec1.z);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(vec2.x, vec2.y, vec2.z);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(result.x, result.y, result.z);
		glEnd();

		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	return 0;
}