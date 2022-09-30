#pragma once
#ifndef __CAMERA
#define __CAMERA
#include <Windows.h> // GetCursorPos/SetCursorPos
#include <gl/GL.h>
#include <gl/glu.h>
#include <math.h>
#include "math.h"

// ------------------- 
// Quaternions camera
// 
// NOT WORK!!!
// ------------------- 
static vector3_t YAW_AXIS = { 0, 1, 0 };
static vector3_t PITCH_AXIS = { 1, 0, 0 };
static vector3_t ROLL_AXIS = { 0, 0, 1 };
	   
static vector3_t FORWARD = { 0, 0, -1 };
static vector3_t UP = { 0, 1, 0 };

#define CAMFL_LIMITANGLES (1 << 0)
#define CAMFL_ACTIVE (1 << 1)
#define CAMFL_KEYBD_CONTROL (1 << 2)
#define CAMFL_DEFAULT (CAMFL_LIMITANGLES|CAMFL_ACTIVE|CAMFL_KEYBD_CONTROL)

typedef struct camera_s {
	float sensitivity;
	float FOV;
	quat_t rot_quat;
	vector3_t rotation;
	vector3_t origin;
	vector3_t direction;
	vector3_t up;
	vector3_t movement;
	float move_scale;
	int screen_width;
	int screen_height;
	int flags;
} camera_t;

static void camera_update_orientation(camera_t *p_camera)
{
	quat_t qyaw, qpitch, qroll, qgav;
	quat_from_angle_axis(&qyaw, p_camera->rotation.yaw, &YAW_AXIS);
	quat_from_angle_axis(&qpitch, p_camera->rotation.pitch, &PITCH_AXIS);
	quat_from_angle_axis(&qroll, p_camera->rotation.roll, &ROLL_AXIS);
	quat_mul(&qgav, &qpitch, &qroll);
	quat_mul(&p_camera->rot_quat, &qyaw, &qgav);
}

#define camera_get_forward_vector(pvecdst, pquat) vec3_rotate_with_quat(pvecdst, &FORWARD, pquat)
#define camera_get_up_vector(pvecdst, pquat) vec3_rotate_with_quat(pvecdst, &UP, pquat)

#define camera_set_screen(c, w, h) \
	(c)->screen_width = w; \
	(c)->screen_height = h;

static void camera_init(camera_t *p_camera, const vector3_t origin,
	const vector3_t direction,
	const vector3_t rotation,
	float FOV, float sensitivity, float movescale,
	int width, int height, int flags)
{
	p_camera->origin = origin;
	p_camera->direction = direction;
	p_camera->rotation = rotation;
	p_camera->FOV = FOV;
	p_camera->sensitivity = sensitivity;
	p_camera->screen_width = width;
	p_camera->screen_height = height;
	p_camera->flags = flags;
	QUAT_SET(&p_camera->rot_quat, 0.f, 0.f, 0.f, 0.f);
	camera_update_orientation(p_camera);
	camera_get_forward_vector(&p_camera->direction, &p_camera->rot_quat);
	camera_get_up_vector(&p_camera->up, &p_camera->rot_quat);
}

static void camera_update_mouse_input(camera_t *p_camera)
{
	POINT pp;
	GetCursorPos(&pp);
	int centerX = p_camera->screen_width >> 1;
	int centerY = p_camera->screen_height >> 1;
	if (pp.x == centerX && pp.y == centerY)
		return;

	p_camera->rotation.yaw -= (centerX - pp.x) * p_camera->sensitivity;
	p_camera->rotation.pitch -= (centerY - pp.y) * p_camera->sensitivity;
	SetCursorPos(centerX, centerY);
}

static void camera_update_vectors(camera_t *p_camera)
{
	vector3_t temp, side;
	camera_get_forward_vector(&p_camera->direction, &p_camera->rot_quat);
	camera_get_up_vector(&p_camera->up, &p_camera->rot_quat);
	vec3_scale(&temp, &p_camera->direction, p_camera->movement.z);
	vec3_add(&p_camera->origin, &p_camera->origin, &temp);

	vec3_scale(&temp, &p_camera->up, p_camera->movement.y);
	vec3_add(&p_camera->origin, &p_camera->origin, &temp);

	vec3_cross(&side, &p_camera->up, &p_camera->direction);
	vec3_normalize(&side);
	vec3_scale(&temp, &side, p_camera->movement.x);
	vec3_add(&p_camera->origin, &p_camera->origin, &temp);
}

static void camera_look(camera_t *p_camera)
{
	if (p_camera->flags & CAMFL_ACTIVE)
		camera_update_mouse_input(p_camera);

	if (p_camera->flags & CAMFL_LIMITANGLES) {
		if (p_camera->rotation.pitch > 89.9f)
			p_camera->rotation.pitch = 89.9f;
		if (p_camera->rotation.pitch < -89.9f)
			p_camera->rotation.pitch = -89.9f;

		if (p_camera->rotation.yaw > 359.9f)
			p_camera->rotation.yaw = 0.f;
		if (p_camera->rotation.yaw < 0.f)
			p_camera->rotation.yaw = 359.9f;
	}

	if (p_camera->flags & CAMFL_KEYBD_CONTROL) {
		if (GetAsyncKeyState('W'))
			p_camera->movement.z = p_camera->move_scale;
		if (GetAsyncKeyState('S'))
			p_camera->movement.z = -p_camera->move_scale;

		if (GetAsyncKeyState('A'))
			p_camera->movement.x = -p_camera->move_scale;
		if (GetAsyncKeyState('D'))
			p_camera->movement.x = p_camera->move_scale;

		if (GetAsyncKeyState(VK_SPACE))
			p_camera->movement.y = p_camera->move_scale;
		if (GetAsyncKeyState(VK_CONTROL))
			p_camera->movement.y = -p_camera->move_scale;
	}
	camera_update_orientation(p_camera);
	camera_update_vectors(p_camera);

	vector3_t look_direction;
	vec3_add(&look_direction, &p_camera->origin, &p_camera->direction);
	gluLookAt(p_camera->origin.x, p_camera->origin.y, p_camera->origin.z, look_direction.x, look_direction.y, look_direction.z, 0.0, 1.0, 0.0);
}

static void camera_set_active(camera_t *p_camera, bool actvation_state)
{
	if (actvation_state) {
		ShowCursor(FALSE);
		p_camera->flags |= CAMFL_ACTIVE;
	}
	else {
		p_camera->flags &= ~CAMFL_ACTIVE;
		ShowCursor(TRUE);
	}
}

static void camera_setup_cursor_center(camera_t *p_camera, HWND hWnd)
{
	RECT wrect;
	GetWindowRect(hWnd, &wrect);
	AdjustWindowRect(&wrect, GetWindowLongPtr(hWnd, GWL_STYLE), (BOOL)GetMenu(hWnd));
	p_camera->screen_width = wrect.right;
	p_camera->screen_height = wrect.bottom;
}

// ----------------------------
// Euler camera
// 
// Pitch Yaw Roll
// x     y   z
// ----------------------------
typedef struct camera2_s {
	HWND window;
	vector3_t origin;
	vector3_t direction;
	vector3_t rotation;
	vector3_t right;
	vector3_t up;
	vector3_t world_up;
	float sensitivity;
	float movement_speed;
	int flags;
} camera2_t;

#define CAMERA_FLAG_ACTIVE (1 << 1)

static void camera2_update_vectors(camera2_t *p_camera)
{
	p_camera->direction.x = cosf(p_camera->rotation.yaw * DTOR) * cosf(p_camera->rotation.pitch * DTOR);
	p_camera->direction.y = sinf(p_camera->rotation.pitch * DTOR);
	p_camera->direction.z = sinf(p_camera->rotation.yaw  * DTOR) * cosf(p_camera->rotation.pitch * DTOR);
	vec3_normalize(&p_camera->direction);

	vec3_cross(&p_camera->right, &p_camera->direction, &p_camera->world_up);
	vec3_normalize(&p_camera->right);
	vec3_cross(&p_camera->up, &p_camera->right, &p_camera->direction);
	vec3_normalize(&p_camera->up);
}

static void camera2_init(camera2_t *p_camera, HWND window, const vector3_t origin,
	const vector3_t rotation,
	const vector3_t direction, const vector3_t world_up_vec, float sensitivity, float movespeed, int flags)
{
	p_camera->window = window;
	p_camera->origin = origin;
	p_camera->rotation = rotation;
	p_camera->direction = direction;
	p_camera->world_up = world_up_vec;
	p_camera->sensitivity = sensitivity;
	p_camera->movement_speed = movespeed;
	p_camera->flags = flags;
	camera2_update_vectors(p_camera);
}

static void camera2_update(camera2_t *p_camera)
{
	POINT pt;
	RECT window_rect;
	GetWindowRect(p_camera->window, &window_rect);
	GetCursorPos(&pt);

	POINT cpt;
	cpt.x = (window_rect.right / 2.f);
	cpt.y = (window_rect.bottom / 2.f);
	float xdiff = pt.x - cpt.x;
	float ydiff = pt.y - cpt.y;

	p_camera->rotation.yaw += xdiff * p_camera->sensitivity;
	p_camera->rotation.pitch += -ydiff * p_camera->sensitivity;
	if (p_camera->rotation.pitch > 89.0f)
		p_camera->rotation.pitch = 89.0f;
	if (p_camera->rotation.pitch < -89.0f)
		p_camera->rotation.pitch = -89.0f;

	vector3_t front_movement, right_movement;
	//if (p_camera->flags & CAMERA_FLAG_ACTIVE) {
		vec3_scale(&front_movement, &p_camera->direction, p_camera->movement_speed);
		vec3_scale(&right_movement, &p_camera->right, p_camera->movement_speed);
		if (GetAsyncKeyState('W'))
			vec3_add(&p_camera->origin, &p_camera->origin, &front_movement);
		if (GetAsyncKeyState('S'))
			vec3_sub(&p_camera->origin, &p_camera->origin, &front_movement);
		if (GetAsyncKeyState('A'))
			vec3_sub(&p_camera->origin, &p_camera->origin, &right_movement);
		if (GetAsyncKeyState('D'))
			vec3_add(&p_camera->origin, &p_camera->origin, &right_movement);

		SetCursorPos(cpt.x, cpt.y);
	//}

	camera2_update_vectors(p_camera);

	vector3_t dir;
	vec3_normalize(&p_camera->direction);
	vec3_add(&dir, &p_camera->origin, &p_camera->direction);

	//printf("org( %f %f %f ) dir( %f %f %f ) rot( %f %f %f )\n",
	//	p_camera->origin.x, p_camera->origin.y, p_camera->origin.z,
	//	p_camera->direction.x, p_camera->direction.y, p_camera->direction.z,
	//	p_camera->rotation.x, p_camera->rotation.y, p_camera->rotation.z
	//);

	gluLookAt(p_camera->origin.x, p_camera->origin.y, p_camera->origin.z, dir.x, dir.y, dir.z, 0.f, 1.f, 0.f);
}


#endif