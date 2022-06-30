#include <stdio.h>
#include "../../common/glmath.h"
#include "../../common/euler_camera.h"
extern "C" {
#include "../../common/gldl.h"
}

gldl_dt_t *pwnd;
Camera camera(true);

float yaw = 0.f, pitch = 0.f;
vec3 angles;
vec3 listener_dir;

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

			case VK_LEFT:
				yaw--;
				break;

			case VK_RIGHT:
				yaw++;
				break;
			}
		}
	};

	if (!(pwnd = gldl_initialize(-1, -1, 800, 600, 24, 32, "Angle between two vectors in space", &events))) {
		printf("Failed to create window!\n");
		return 1;
	}

	glViewport(0, 0, pwnd->GetWindowWidth(), pwnd->GetWindowHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, pwnd->GetWindowWidth() / (double)pwnd->GetWindowHeight(), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vec3 source = { 20, 0, 20 };
	vec3 pos = { 0, 0, 0 };

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera.Look(pwnd->GetWindow());

		glPushAttrib(GL_CURRENT_BIT);

		glColor4ub(255, 255, 255, 40);
		pwnd->Draw3DGird(0, 0, 0, 100, 5);

		glColor4ub(0, 255, 0, 255);
		pwnd->DrawSphere(source.x, source.y, source.z, 0.5f);

		listener_dir.x = cosf(yaw * DTOR) * cosf(pitch * DTOR);
		listener_dir.y = sinf(pitch * DTOR);
		listener_dir.z = sinf(yaw  * DTOR) * cosf(pitch * DTOR);
		
		vec3 endpoint = pos + (listener_dir * 1000);
		glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(endpoint.x, endpoint.y, endpoint.z);
		glEnd();

		float cos_angle = cos_angle_between_vectors(source, listener_dir);

		static vec3 vn(0.f, 1.f, 0.f);
		float angle2 = angle_between_vectors(source, listener_dir, vn);

		// clamp value -1.f -- 1.f
		if (cos_angle < -1.f)
			cos_angle = -1.f;
		if (cos_angle > 1.f)
			cos_angle = 1.f;

		float angle = acosf(cos_angle);
		printf("Angle180: %f   Angle360: %f\n", angle * RTOD, angle2 * RTOD);

		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	return 0;
}