#include <stdio.h>
#include "../../common/math.h"
#include "../../common/gldl.h"
#include "../../common/camera.h"
#include "../../common/perlin.h"

#define DEMO_SCREEN_WIDTH  1280
#define DEMO_SCREEN_HEIGHT 1024

#define MOVE_SPEED  0.001
#define SENSITIVITY 0.1

#define DEG2RAD 0.0174f
#define RAD2DEG 57.3248f

#define MAX_POLYGON_POINTS 128

enum MODES {
	MODE_EDIT = 0,
	MODE_TEST,

	MODE_MAX // not use this for MODE set
};

typedef struct color3_s {
	unsigned char r, g, b, a;
} color3_t;

#define color_set(dst, _r, _g, _b, _a) \
	(dst)->r = _r; \
	(dst)->g = _g; \
	(dst)->b = _b; \
	(dst)->a = _a;

typedef struct vertex_s {
	vector3_t pt;
	color3_t color;
} vertex_t;

int            mode = MODE_EDIT;
gldl_dt_t     *pwnd = NULL;
camera2_t      camera;
vector3_t      plane_rotation = { 0.f, 0.f, 0.f };
vector3_t      plane_normal = { 0.f, 0.f, 0.f };
vector3_t      center = { 0.f, 0.f, 0.f };

size_t         points_count = 0;
vertex_t       polygon_points[MAX_POLYGON_POINTS];

void normal_by_angles(vector3_t *p_normal, const vector3_t *p_angles)
{
	p_normal->x = cosf(p_angles->yaw * DTOR) * cosf(p_angles->pitch * DTOR);
	p_normal->y = sinf(p_angles->pitch * DTOR);
	p_normal->z = sinf(p_angles->yaw  * DTOR) * cosf(p_angles->pitch * DTOR);
	vec3_normalize(p_normal);
}

// Newell's algorithm
//void compute_polygon_normal(vector3_t *p_dstnormal, const vector3_t *pts, int num)
//{
//	p_dstnormal->x = p_dstnormal->y = p_dstnormal->z = 0.f;
//	for (int i = 0; i < num; i++) {
//		vector3_t *p1 = &pts[i];
//		vector3_t *p2 = &pts[(i + 1) % num];
//		p_dstnormal->x += (p1->y - p2->y) * (p1->z + p2->z);
//		p_dstnormal->y += (p1->z - p2->z) * (p1->x + p2->x);
//		p_dstnormal->z += (p1->x - p2->x) * (p1->y + p2->y);
//	}
//	vec3_normalize(p_dstnormal);
//}

void compute_polygon_normal(vector3_t *p_dstnormal, const vertex_t *pts, int num)
{
	p_dstnormal->x = p_dstnormal->y = p_dstnormal->z = 0.f;
	for (int i = 0; i < num; i++) {
		vector3_t *p1 = &pts[i].pt;
		vector3_t *p2 = &pts[(i + 1) % num].pt;
		p_dstnormal->x += (p1->y - p2->y) * (p1->z + p2->z);
		p_dstnormal->y += (p1->z - p2->z) * (p1->x + p2->x);
		p_dstnormal->z += (p1->x - p2->x) * (p1->y + p2->y);
	}
	vec3_normalize(p_dstnormal);
}


// limit deg angles 0 - 359
void correct_angles(float *p_angle)
{
	if (*p_angle > 359.999) {
		*p_angle = 0.f;
		return;
	}

	if (*p_angle < 0.f)
		*p_angle = 359.999;
}

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
			static const char *p_modes[] = {
				"POLYGON EDIT MODE",
				"PLANE NORMAL COMPUTATION TEST"
			};
			
			pwnd->SetTitle("Polygon plane normal (%s)", p_modes[mode]);
			mode = (mode + 1) % MODE_MAX;
			break;
		}

		case VK_F2: {
			break;
		}

		// add new polygon point
		case VK_RETURN: {
			if (mode == MODE_EDIT) {
				vector3_t intersect_point;
				normal_by_angles(&plane_normal, &plane_rotation);
				if (PlaneIntersection5(&center, &plane_normal, &camera.origin, &camera.direction, &intersect_point)) {
					if (points_count >= MAX_POLYGON_POINTS) {
						printf("Points buffer is full\n");
						return;
					}
					printf("Saved point ( %f %f %f ) at index %d\n", intersect_point.x, intersect_point.y, intersect_point.z, points_count);

					vertex_t *p_vertex = &polygon_points[points_count];
					p_vertex->pt = intersect_point;
					color_set(&p_vertex->color, 255, 255, 255, 255);
					points_count++;
				}
			}
			break;
		}

		case VK_BACK:
			if (points_count > 0) {
				vector3_t *p_pt = &polygon_points[points_count].pt;
				printf("Last point ( %f %f %f ) was deleted at index %d\n", p_pt->x, p_pt->y, p_pt->z, points_count);
				points_count--;
			}
			break;

		case VK_DELETE:
			points_count = 0;
			printf("All points has deleted!\n");
			break;

		case VK_UP:
			plane_rotation.x += 0.9f;
			correct_angles(&plane_rotation.x);
			normal_by_angles(&plane_normal, &plane_rotation);
			printf("Rotation around X+: angles( %f %f %f )  normal( %f %f %f )\n",
				plane_rotation.x, plane_rotation.y, plane_rotation.z,
				plane_normal.x, plane_normal.y, plane_normal.z
			);
			break;

		case VK_DOWN:
			plane_rotation.x -= 0.9f;
			correct_angles(&plane_rotation.x);
			normal_by_angles(&plane_normal, &plane_rotation);
			printf("Rotation around X-: angles( %f %f %f )  normal( %f %f %f )\n",
				plane_rotation.x, plane_rotation.y, plane_rotation.z,
				plane_normal.x, plane_normal.y, plane_normal.z
			);
			break;

		case VK_LEFT:
			plane_rotation.y -= 0.9f;
			correct_angles(&plane_rotation.y);
			normal_by_angles(&plane_normal, &plane_rotation);
			printf("Rotation around Y-: angles( %f %f %f )  normal( %f %f %f )\n",
				plane_rotation.x, plane_rotation.y, plane_rotation.z,
				plane_normal.x, plane_normal.y, plane_normal.z
			);
			break;

		case VK_RIGHT:
			plane_rotation.y += 0.9f;
			correct_angles(&plane_rotation.y);
			normal_by_angles(&plane_normal, &plane_rotation);
			printf("Rotation around Y+: angles( %f %f %f )  normal( %f %f %f )\n",
				plane_rotation.x, plane_rotation.y, plane_rotation.z,
				plane_normal.x, plane_normal.y, plane_normal.z
			);
			break;
		}
	}
}

void draw_plane(const vector3_t *p_origin, const vector3_t *p_angles, float width)
{
	vertex_t *p_vertex;
	vertex_t plane_vertices[4];

	// vertex 0
	p_vertex = &plane_vertices[0];
	color_set(&p_vertex->color, 255, 255, 255, 50);
	p_vertex->pt.x = -width;
	p_vertex->pt.y = 0.f;
	p_vertex->pt.z = -width;

	// vertex 1
	p_vertex = &plane_vertices[1];
	color_set(&p_vertex->color, 255, 255, 255, 50);
	p_vertex->pt.x = -width;
	p_vertex->pt.y = 0.f;
	p_vertex->pt.z = width;

	// vertex 2
	p_vertex = &plane_vertices[2];
	color_set(&p_vertex->color, 255, 255, 255, 50);
	p_vertex->pt.x = width;
	p_vertex->pt.y = 0.f;
	p_vertex->pt.z = width;

	// vertex 3
	p_vertex = &plane_vertices[3];
	color_set(&p_vertex->color, 255, 255, 255, 50);
	p_vertex->pt.x = width;
	p_vertex->pt.y = 0.f;
	p_vertex->pt.z = -width;

	glEnable(GL_BLEND);
	glPushMatrix();
	
	glRotatef(p_angles->x, 1.f, 0.f, 0.f);
	glRotatef(p_angles->y, 0.f, 1.f, 0.f);
	glRotatef(p_angles->z, 0.f, 0.f, 1.f);
	glTranslatef(p_origin->x, p_origin->y, p_origin->z);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glVertexPointer(3, GL_FLOAT, sizeof(vertex_t), ((char *)plane_vertices + offsetof(vertex_t, pt)));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vertex_t), ((char *)plane_vertices + offsetof(vertex_t, color)));
	glDrawArrays(GL_QUADS, 0, 4);
	glPopMatrix();
	glDisable(GL_BLEND);
}

int main()
{
	gldl_events_dt_t events;
	memset(&events, 0, sizeof(events));
	events.p_keydown_input_event = keydown;
	if (!(pwnd = gldl_initialize(-1, -1, DEMO_SCREEN_WIDTH, DEMO_SCREEN_HEIGHT, 24, 32, "Polygon plane normal", &events))) {
		printf("Failed to create window!\n");
		return 1;
	}

	camera2_init(&camera, pwnd->GetWindow(),
		(vector3_t) { 0.f, 0.f, 0.f },
		(vector3_t) { 0.f, 0.f, 0.f },
		(vector3_t) { 0.f, 0.f, 0.f },
		(vector3_t) { 0.f, 1.0, 0.f },
		SENSITIVITY,
		MOVE_SPEED,
		0
	);

	normal_by_angles(&plane_normal, &plane_rotation);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glBegin(GL_POINTS);
		glVertex3f(0.f, 0.f, 0.f);
		glEnd();

		camera2_update(&camera);
		pwnd->Draw3DAxises(1.0f, 1.0f);
		//pwnd->Draw3DGird(0.f, 0.f, 0.f, 10.f, 10.f);

		glPushAttrib(GL_CURRENT_BIT);
		if (mode == MODE_EDIT) {
			static vector3_t plane_origin = { 0.f, 0.f, 0.f };
			draw_plane(&plane_origin, &plane_rotation, 50.f);
		}

		if (points_count > 2) {
			vector3_t polygon_normal, line_endpoint;
			glVertexPointer(3, GL_FLOAT, sizeof(vertex_t), ((char *)polygon_points + offsetof(vertex_t, pt)));
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vertex_t), ((char *)polygon_points + offsetof(vertex_t, color)));
			glDrawArrays(GL_LINE_LOOP, 0, points_count);
			compute_polygon_normal(&polygon_normal, polygon_points, points_count);

			vec3_scale(&line_endpoint, &polygon_normal, 5.f);
			vec3_add(&line_endpoint, &polygon_points[0].pt, &line_endpoint);

			glColor3ub(255, 0, 0);
			glBegin(GL_LINES);
			glVertex3fv((float *)&polygon_points[0].pt);
			glVertex3fv((float *)&line_endpoint);
			glEnd();
			
		}
		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	return 0;
}