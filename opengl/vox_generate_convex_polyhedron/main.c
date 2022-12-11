#include <stdio.h>
#include "../../common/math.h"
#include "../../common/gldl.h"
#include "../../common/camera.h"
#include "../../common/perlin.h"
#include "../../common/voxgroup/voxel.h"

#define DEMO_SCREEN_WIDTH  1280
#define DEMO_SCREEN_HEIGHT 1024

#define MOVE_SPEED  0.009
#define SENSITIVITY 0.1

#define X_MAX 20
#define Y_MAX 15
#define Z_MAX 20
#define MAX_VOXELS ((X_MAX + 1) * (Y_MAX + 1) * (Z_MAX + 1))
#define MAX_POINTS MAX_VOXELS //points == voxels

#define DM_ALL   0
#define DM_SOLID 1
#define DM_AIR   2
#define DM_MAX   3 //not use for selection

#define NOISE_FREQ  0.00001
#define NOISE_DEPTH 10.0
#define NOISE_SCALE 7.0

#define SCALE_ADDITION 0.1f
#define FREQ_ADDITION 0.001f

typedef struct color3_s {
	unsigned char r, g, b;
} color3_t;

typedef struct point3d_s {
	vector3_t point;
	color3_t color;
} point3d_t;

float          scale = NOISE_SCALE;
float          frequency = NOISE_FREQ;
int            display_mode = DM_ALL;
gldl_dt_t     *pwnd = NULL;
size_t         number_of_points = 0;
point3d_t     *p_convex_points = NULL;
camera2_t      camera;
voxel_chunk_t  chunk;

void voxels_generate(float freq, float scale, float depth);

void keydown(int keycode, int state)
{
	if (state == STATE_PRESSED) {
		switch (keycode) {

		case VK_ESCAPE: {
			HWND hwnd = pwnd->GetWindow();
			SendMessageA(hwnd, WM_CLOSE, 0, 0);
			break;
		}

		case VK_F1: {
			static const char *p_descriptions[] = {
				"ALL DISPLAY MODE",
				"SOLID DISPLAY MODE",
				"AIR DISPLAY MODE"
			};
			display_mode = (display_mode + 1) % DM_MAX;
			printf("%s\n", p_descriptions[display_mode]);
			voxels_generate(frequency, scale, NOISE_DEPTH);
			break;
		}

		case VK_F2: {
			printf("Regenerate voxels...\n");
			voxels_generate(frequency, scale, NOISE_DEPTH);
			break;
		}

		case VK_UP:
			scale += SCALE_ADDITION;
			printf("Noise scale: %f\n", scale);
			voxels_generate(frequency, scale, NOISE_DEPTH);
			break;

		case VK_DOWN:
			if(scale > SCALE_ADDITION)
				scale -= SCALE_ADDITION;

			printf("Noise scale: %f\n", scale);
			voxels_generate(frequency, scale, NOISE_DEPTH);
			break;

		case VK_LEFT:
			if (frequency > FREQ_ADDITION)
				frequency -= FREQ_ADDITION;

			printf("Frequency: %f\n", frequency);
			voxels_generate(frequency, scale, NOISE_DEPTH);
			break;

		case VK_RIGHT:
			frequency += FREQ_ADDITION;
			printf("Frequency: %f\n", frequency);
			voxels_generate(frequency, scale, NOISE_DEPTH);
			break;
		}
	}
}

inline void points_reset()
{
	number_of_points = 0;
}

inline void point_add(const vector3_t point, const color3_t color)
{
	if (number_of_points < MAX_POINTS) {
		p_convex_points[number_of_points].point = point;
		p_convex_points[number_of_points].color = color;
		number_of_points++;
		return;
	}
	printf("point_add(): buffer overflowed\n");
}

void voxels_generate(float freq, float scale, float depth)
{
	points_reset(); //reset points buffer
	for (size_t y = 0; y <= Y_MAX; y++) {
		for (size_t x = 0; x <= X_MAX; x++) {
			for (size_t z = 0; z <= Z_MAX; z++) {
				static const color3_t colors[] = {
					{ 255, 0, 0 },
					{ 0, 0, 255 }
				};

				vector3_t vec_float;
				vec_float.x = (float)x;
				vec_float.y = (float)y;
				vec_float.z = (float)z;
				float noise_val = perlin2d(vec_float.x, vec_float.z, freq, depth) * scale;
				bool is_solid = (bool)(noise_val <= vec_float.y); //this voxel is solid?

				voxel_t *p_voxel = vox_get(&chunk, x, y, z);
				if (!p_voxel)
					continue;

				p_voxel->data |= (is_solid) ? VF_SOLID : 0;

				// generate voxels visualize points
				if (is_solid && display_mode == DM_SOLID)
					point_add((vector3_t) { (float)x, (float)y, (float)z }, colors[is_solid]);

				else if (!is_solid && display_mode == DM_AIR)
					point_add((vector3_t) { (float)x, (float)y, (float)z }, colors[is_solid]);

				else if (display_mode == DM_ALL)
					point_add((vector3_t) { (float)x, (float)y, (float)z }, colors[is_solid]);
			}
		}
	}
}

int main()
{
	//while (1) {
	//	//Beep(1900, 50);
	//	//Beep(2000, 50);
	//	//Beep(1800, 50);


	//	Beep(1500, 500);
	//	Beep(2000, 500);
	//	Beep(1500, 500);
	//	Beep(2000, 500);

	//	//Sleep(100);
	//}


	vec2_t vectors[] = {
		{ 1.f, 0.f }, 
		{ 0.f, 1.f }, 
	};
	float angle = acosf(vec2_dot(&vectors[0], &vectors[1]));
	printf("DEG: %f\n", angle * RTOD);

	vec2_t ray;
	vec2_t pt = { 0.5f, 0.5f };
	vec2_t dst = { 1.f, 0.f };
	vec2_add(&dst, &pt, &dst);
	vec2_normalize(&ray, &dst);
	
	printf("Ray direction: ( %f %f )\n", ray.x, ray.y);

	/* line intersection test */
	vec2_t diff;
	vec2_sub(&diff, &vectors[1], &vectors[0]);

	float ang_rad = fabsf(vec2_dot(&ray, &diff));
	printf("angle radians: %f\n", ang_rad);
	if (ang_rad > EPSILON) {
		printf("RAY INTERSECT LINE!\n");
	}

	return 0;

	gldl_events_dt_t events;
	memset(&events, 0, sizeof(events));
	events.p_keydown_input_event = keydown;
	if (!(pwnd = gldl_initialize(-1, -1, DEMO_SCREEN_WIDTH, DEMO_SCREEN_HEIGHT, 24, 32, "Generate convex polyhedron regions from voxels", &events))) {
		printf("Failed to create window!\n");
		return 1;
	}

	if (!vox_chunk_alloc(&chunk, X_MAX, Y_MAX, Z_MAX)) {
		printf("Failed to allocate voxels!\n");
		return 1;
	}

	p_convex_points = (point3d_t *)malloc(sizeof(point3d_t) * MAX_POINTS);
	if (!p_convex_points) {
		printf("Failed to allocate memory!\n");
		return 1;
	}

	camera2_init(&camera, pwnd->GetWindow(), (vector3_t) {0.f, 0.f, 0.f}, (vector3_t) {0.f, 0.f, 0.f}, (vector3_t) {0.f, 0.f, 0.f}, (vector3_t) {0.f, 1.0, 0.f}, SENSITIVITY, MOVE_SPEED, 0);
	voxels_generate(scale, scale, NOISE_DEPTH);

	glPointSize(5.f);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glPushAttrib(GL_CURRENT_BIT);
		//glTranslatef(0.f, 0.f, -80.f);
		//glRotatef(30.f, 0.f, 1.f, 0.f);
		//glRotatef(15.f, 1.f, 0.f, 0.f);

		camera2_update(&camera);
		pwnd->Draw3DAxises(1.0f, 1.0f);
		pwnd->Draw3DGird(0.f, 0.f, 0.f, X_MAX / 2, Z_MAX / 2);

		// draw points
		glVertexPointer(3, GL_FLOAT, sizeof(point3d_t), ((char*)p_convex_points + offsetof(point3d_t, point)));
		glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(point3d_t), ((char*)p_convex_points + offsetof(point3d_t, color)));
		glDrawArrays(GL_POINTS, 0, number_of_points);

		
		glPopAttrib();
	}
	free(p_convex_points);
	vox_chunk_free(&chunk);
	gldl_shutdown(pwnd);
	return 0;
}