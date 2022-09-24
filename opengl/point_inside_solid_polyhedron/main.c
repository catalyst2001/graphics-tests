#include <stdio.h>
#include "../../common/math.h"
#include "../../common/gldl.h"

gldl_dt_t *pwnd;

#define CD 10

vector3_t polyhedron_coords[] = {
	{CD, -CD, -CD},
	{CD, -CD, CD},
	{CD, CD, -CD},
	{CD, CD, CD},
	{-CD, CD, -CD},
	{-CD, CD, CD},
	{-CD, -CD, -CD},
	{-CD, -CD, CD},
};

vector3_t point_coord = { 0.f, 0.f, 0.f };

void keydown(int keycode, int state)
{
	if (state == STATE_PRESSED) {
		switch (keycode) {
		case VK_LEFT:
			point_coord.x += 1;
			break;

		case VK_RIGHT:
			point_coord.x -= 1;
			break;

		case VK_UP:
			point_coord.z += 1;
			break;

		case VK_DOWN:
			point_coord.z -= 1;
			break;

		case VK_ADD:
			point_coord.y += 1;
			break;

		case VK_SUBTRACT:
			point_coord.y -= 1;
			break;
		
		case VK_ESCAPE: {
			HWND hwnd = pwnd->GetWindow();
			SendMessageA(hwnd, WM_CLOSE, 0, 0);
			break;
		}

		}
	}
}

static vector3b_t colors[] = {
	{ 255, 0, 0 },
	{ 0, 255, 0 }
};

bool point_inside_solid_polyhedron(const vector3_t *p_point, const vector3_t *p_coords, size_t coords_count)
{
	vector3_t normal;
	for (size_t i = 0; i < coords_count; i++) {
		vec3_cross(&normal, p_point, &p_coords[i]);
		vec3_normalize(&normal);
		//printf("coord( %f %f %f ) point(%f %f %f) == normal( %f %f %f )\n",
		//	p_coords[i].x, p_coords[i].y, p_coords[i].z, 
		//	p_point->x, p_point->y, p_point->z,
		//	normal.x, normal.y, normal.z);
	
		if (normal.x < 0.f && normal.y < 0.f && normal.z < 0.f)
			return false;
	}
	//printf("\n\n");
	return true;
}

int main()
{
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));
	//point_coord.x += 10;
	//return 0;


	gldl_events_dt_t events;
	memset(&events, 0, sizeof(events));
	events.p_keydown_input_event = keydown;
	if (!(pwnd = gldl_initialize(-1, -1, 1280, 1024, 24, 32, "Point Inside Solid Polyhedron", &events))) {
		printf("Failed to create window!\n");
		return 1;
	}

	glPointSize(5.f);
	glEnableClientState(GL_VERTEX_ARRAY);
	while (pwnd->IsWindowOpened()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glPushAttrib(GL_CURRENT_BIT);
		pwnd->Draw3DAxises(1.0f, 1.0f);
		glTranslatef(0.f, 0.f, -80.f);
		glRotatef(30.f, 0.f, 1.f, 0.f);
		glRotatef(15.f, 1.f, 0.f, 0.f);

		glVertexPointer(3, GL_FLOAT, 0, polyhedron_coords);
		glDrawArrays(GL_POINTS, 0, 8);

		bool b_inside = point_inside_solid_polyhedron(&point_coord, polyhedron_coords, sizeof(polyhedron_coords) / sizeof(polyhedron_coords[0]));

		glColor3ubv(&colors[b_inside]);
		glVertexPointer(3, GL_FLOAT, 0, &point_coord);
		glDrawArrays(GL_POINTS, 0, 1);
		glPopAttrib();
	}
	gldl_shutdown(pwnd);
	return 0;
}