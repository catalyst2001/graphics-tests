#pragma once
//----------------------------------------------
// GL Developer Library
// ���������� ��������� ���������� ���� ���������� � ������������� OpenGL ��� �������������� � ������ �����
// �����: ������� �
// ����: 05.03.2022
// 
// ��������� �� ������ ������ ���������� � �� ��������� �� ������������� � �������� ��������!!!
// �����:
// - ������ � �������������
// - �� �������� � ��������� DLL ��� glut, glfw, � ��� ������ ��� ��� ����� ������� � ���� ���� ���������� ���
//   ����������� ���������� ���������. ��� �������� ������������� ������� � �� ������� �������������� ���������.
// - ����� ���������� ����� ��� ��������� ������ (��� OpenGL �� ���������� ����)
// - ����� ���������� ������, ����� ��� ����� � ������� (glu), �������������� �������������, 3D ����� � ������������ ����
//   ���������� ������� ��������� ������������ � �� ������� ����������.
//----------------------------------------------
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

/**
-----------------------------------------------
	���� ���������� �� ������� ��� dispatch table
	� ������� ����� �� ��������������
-----------------------------------------------
*/
typedef void (*begin2d_fn)();
typedef void (*end2d_fn)();
typedef void (*sphere_fn)(float x, float y, float z, float radius);
typedef void (*cylinder_fn)(float x, float y, float z, float width, float height);
typedef void (*bound_box_fn)(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax);
typedef void (*gird_3d)(float x, float y, float z, float width, float step);
typedef void (*draw_axises)(float line_width, float axis_length);
typedef bool (*is_window_active)();
typedef bool (*is_button_pressed)(int button);

typedef void (*print_text_fn)(int x, int y, const char *p_text, ...);
typedef long (*int_return_fn)();

typedef struct mouse_point_s {
	long x, y;
} mouse_point_t;
typedef void (*point_ptr_fn)(mouse_point_t *p_dest);

typedef struct gldl_dt_s {

	//������ � ��������� ��������� � ������������� ��������
	begin2d_fn        Begin2D;
	end2d_fn          End2D;

	//����������������� ������
	sphere_fn         DrawSphere;
	cylinder_fn       DrawCylinder;
	bound_box_fn      DrawBBox;
	gird_3d           Draw3DGird;
	draw_axises       Draw3DAxises;

	//������� ����
	is_window_active  IsWindowOpened;
	int_return_fn     GetWindowPosX;
	int_return_fn     GetWindowPosY;
	int_return_fn     GetWindowWidth;
	int_return_fn     GetWindowHeight;
	int_return_fn     GetColorBits;
	int_return_fn     GetDepthBits;

	//������ �� ������ ���������� ��� ����
	is_button_pressed IsButtonPressed;
	is_button_pressed IsButtonPressedOne;
	int_return_fn     GetMouseWheelDelta;

	//�������� ����� �� ������
	print_text_fn     PrintText;

	//�������� ������� ������� ������ ����
	int_return_fn     GetMouseX;
	int_return_fn     GetMouseY;
	point_ptr_fn      GetMousePosition;

	HWND              (*GetWindow)();

} gldl_dt_t;

//������� �������
typedef struct gldl_events_dt_s {
	void(*p_mouse_move_event)(int x, int y);
	void(*p_mouse_click_event)(int x, int y, int button, int state);
	void(*p_mouse_wheel_event)(int delta);
	void(*p_char_input_event)(int keycode);
	void(*p_keydown_input_event)(int keycode, int state);
	void(*p_window_resize_event)(int width, int height);
} gldl_events_dt_t;

#define LBUTTON 1020 //����� ������ ����
#define RBUTTON 1021 //������ ������ ����
#define MBUTTON 1022 //����������� ������ ����
#define STATE_PRESSED 0 //������ ������
#define STATE_RELEASED 1 //������ ��������

// ���� ������
typedef enum gldl_codes_e {
	GLDL_OK = 0, //��� ������
	GLDL_ERROR_CREATING_WCLASS,
	GLDL_ERROR_CREATING_WINDOW,
	GLDL_ERROR_GET_DEVICE_CONTEXT,
	GLDL_ERROR_CHOOSE_PIXELFORMAT,
	GLDL_ERROR_SET_PIXEL_FORMAT,
	GLDL_ERROR_CREATING_GL_CONTEXT,
	GLDL_ERROR_MAKE_CURRENT_CONTEXT
} gldl_codes_t;

// 
// gldl_initialize
//  x - ������� x ���� �� ������. ���� x ��� y ���������� � -1, ������� ���� ����� ��������� �� ������ ������
//  y - ������� y ���� �� ������. ���� x ��� y ���������� � -1, ������� ���� ����� ��������� �� ������ ������
//  width - ������ ����
//  height - ������ ����
//  color_bits - ���������� ��� �� ������� �����. �� ��������� RGB (24 ����)
//  depth_bits - ���������� ��� ������ �������. �� ��������� 32 ����
//  p_events_dispatch_table - ������� � ��������� ��������� �������, ����� ��� ���� ����, ������� ������, �������� ���� � ����
//                            ��������� ������� ����. ����� �� �������������� � ���� NULL !
//  
// �������������� ����������� ���������� OpenGL � ������� ���� ��� ��������� �������
// ��� �������� ����������, ������� ������ ����� ������� �������, ��������� ��� �������������
// ���� ������������ �������� ����� ���� (NULL), ��������� ��������� ��� ��������� ������������ ������
// 
gldl_dt_t *gldl_initialize(int x, int y, int width, int height, int color_bits, int depth_bits, const char *p_title, gldl_events_dt_t *p_events_dispatch_table);
gldl_codes_t gldl_last_error();
gldl_codes_t gldl_shutdown(gldl_dt_t *p_gldl_window);