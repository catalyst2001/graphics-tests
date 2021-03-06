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

void print_uho(float deg)
{
	if (deg == 0.f) {
		printf("( R: 0.5  | L: 0.5 )\n");
	}
	else {
		printf("( R: %f  | L: %f )\n", (deg / 180.f) * 2,  1 - ((deg / 180.f) * 2));
	}
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

	vec3 source_pos = { 20, 0, 0 };
	float source_radius = 50.f;

	vec3 listener_pos = { 0, 0, 0 };

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		camera.Look(pwnd->GetWindow());

		glPushAttrib(GL_CURRENT_BIT);

		glColor4ub(255, 255, 255, 40);
		pwnd->Draw3DGird(0, 0, 0, 100, 5);

		glColor4ub(0, 255, 0, 255);
		pwnd->DrawSphere(source_pos.x, source_pos.y, source_pos.z, 0.5f);

		//listener_dir.x = cosf(yaw * DTOR) * cosf(pitch * DTOR);
		//listener_dir.y = sinf(pitch * DTOR);
		//listener_dir.z = sinf(yaw  * DTOR) * cosf(pitch * DTOR);
		//
		//vec3 endpoint = listener_pos + (listener_dir * 1000);
		//glBegin(GL_LINES);
		//glVertex3f(listener_pos.x, listener_pos.y, listener_pos.z);
		//glVertex3f(endpoint.x, endpoint.y, endpoint.z);
		//glEnd();

		//float cos_angle = clampf(cos_angle_between_vectors(source_pos, listener_dir), -1.f, 1.f); // clamp value -1.f -- 1.f
		//float angle = acosf(cos_angle);

		//static vec3 vn(0.f, 1.f, 0.f);
		//float angle2 = angle_between_vectors(source_pos, listener_dir, vn);
		//printf("Angle180: %f   Angle360: %f  -1 -- 1: %f  ",
		//	angle * RTOD,
		//	angle2 * RTOD,
		//	(angle2 * RTOD) / 180.f);

		///////////////////////////////////////////////
		vec3 ear;
		vec3 up_vec(0.f, 1.f, 0.f);
		vec3 listener_look = camera.Front;
		cross(ear, listener_look, up_vec);
		ear.normalize();
		up_vec.normalize();
		listener_look.normalize();

		listener_pos = camera.Position;
		
		vec3 raznost = source_pos - listener_pos;
		float distance2 = raznost.length2();
		float pan = 0.f;
		if (distance2 < 0.000000001) {
			pan = 0.f;
		}
		else {
			raznost.normalize();
			pan = dot(raznost, ear);
		}

		float dist_gain = 1.f / (1.f + (distance2 / (source_radius * source_radius)));
		float left_gain = dist_gain * (1.f + pan);
		float right_gain = dist_gain * (1.f - pan);
		printf("dist_gain=%f  left_gain=%f  right_gain=%f\n", dist_gain, left_gain, right_gain);

		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	return 0;
}