#include <stdio.h>
#include "../../common/math.h"
#include "../../common/camera.h"
#include "../../common/gldl.h"

gldl_dt_t *pwnd;
camera2_t camera;

int main()
{
	if (!(pwnd = gldl_initialize(-1, -1, 800, 600, 24, 32, "ray sphere intersect", NULL))) {
		printf("Failed to create window!\n");
		return 1;
	}

	camera.origin = VEC3(0.f, 0.f, 0.f);
	camera.direction = VEC3(0.f, 0.f, 0.f);
	camera.sensitivity = 0.1f;
	RECT wrect;
	GetWindowRect(pwnd->GetWindow(), &wrect);
	AdjustWindowRect(&wrect, GetWindowLongPtr(pwnd->GetWindow(), GWL_STYLE), (BOOL)GetMenu(pwnd->GetWindow()));
	camera.mousecenter.x = wrect.right / 2;
	camera.mousecenter.y = wrect.bottom / 2;

	glViewport(0, 0, pwnd->GetWindowWidth(), pwnd->GetWindowHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.f, pwnd->GetWindowWidth() / (double)pwnd->GetWindowHeight(), 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		ecamera_update(&camera);

		pwnd->Draw3DAxises(2.f, 2.f);
		pwnd->Draw3DGird(0, 0, 0, 100, 5);
	}
	gldl_shutdown(pwnd);
	return 0;
}