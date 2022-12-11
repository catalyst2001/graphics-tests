#include <Windows.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>


#include "../../common/math.h"

#define WINDOW_CLASS "WindowClass"

HINSTANCE g_instance;
HWND h_wnd;

POINT cursor_pt;

HPEN h_pen_ray;
HPEN h_pen_poly;
HBRUSH h_white_brush;

#define MAX_POLYGON_COORDS 128

int curr_poly_coord = 0;
vec2_t poly_coords[MAX_POLYGON_COORDS];

void ErrorMessage(const char *p_format, ...);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = WINDOW_CLASS;
	if (!RegisterClassExA(&wcex)) {
		ErrorMessage("Couldn't create window class");
		return 1;
	}

	g_instance = hInstance;
	INT width = 800;
	INT height = 600;
	INT xpos = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
	INT ypos = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);
	h_wnd = CreateWindowExA(0, wcex.lpszClassName, "Window Application", WS_OVERLAPPEDWINDOW, xpos, ypos, width, height, NULL, NULL, hInstance, NULL);
	if (!h_wnd) {
		ErrorMessage("Couldn't create window");
		return 2;
	}

	h_pen_ray = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	h_pen_poly = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	h_white_brush = CreateSolidBrush(RGB(255, 255, 255));

	ShowWindow(h_wnd, nCmdShow);
	UpdateWindow(h_wnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
	DeleteObject(h_pen_ray);
	DeleteObject(h_pen_poly);
	DeleteObject(h_white_brush);
    return (int) msg.wParam;
}

void draw_line(HDC hdc, int xfrom, int yfrom, int xto, int yto)
{
	POINT oldpt;
	MoveToEx(hdc, xfrom, yfrom, &oldpt);
	LineTo(hdc, xto, yto);
	MoveToEx(hdc, oldpt.x, oldpt.y, &oldpt);
}

void draw_polygon(HDC hdc, vec2_t *p_coords, int num_coords)
{
	if (num_coords < 1)
		return;

	POINT oldpt;
	MoveToEx(hdc, (int)p_coords[0].x, (int)p_coords[0].y, &oldpt);
	for (int i = 1; i < num_coords; i++)
		LineTo(hdc, (int)p_coords[i].x, (int)p_coords[i].y);
	
	LineTo(hdc, (int)p_coords[0].x, (int)p_coords[0].y);
	MoveToEx(hdc, oldpt.x, oldpt.y, &oldpt);
}

void draw_textf(HDC hdc, int x, int y, const char *p_format, ...)
{
	va_list argptr;
	char buffer[512];
	va_start(argptr, p_format);
	vsprintf_s(buffer, sizeof(buffer), p_format, argptr);
	va_end(argptr);

	TextOutA(hdc, x, y, buffer, strlen(buffer));
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT current_rect;
	static HBITMAP h_bitmap = NULL;
	static HDC h_chdc = NULL;
	static RECT rect;
    switch (message)
    {
		case WM_CREATE: {
			HDC hdc = GetDC(hWnd);
			h_chdc = CreateCompatibleDC(hdc);
			GetClientRect(hWnd, &rect);
			h_bitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(h_chdc, h_bitmap);
			SetBkMode(h_chdc, TRANSPARENT);

			ReleaseDC(hWnd, hdc);
			return 0;
		}

		case WM_SIZE: {
			SIZE window_size;
			window_size.cx = LOWORD(lParam);
			window_size.cy = HIWORD(lParam);
			if (window_size.cy == 0)
				window_size.cy = 1;

			GetClientRect(hWnd, &current_rect);
			if (!EqualRect(&current_rect, &rect)) {
				CopyRect(&rect, &current_rect);
				HDC hdc = GetDC(hWnd);
				if (h_bitmap)
					DeleteObject(h_bitmap);

				h_bitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
				SelectObject(h_chdc, h_bitmap);
				ReleaseDC(hWnd, hdc);
			}
			break;
		}

		case WM_DESTROY:
			DeleteDC(h_chdc);
			DeleteObject(h_bitmap);
			PostQuitMessage(0);
			break;

		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			break;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps;
			char buffer[128];
			HGDIOBJ h_oldpen;
			vec2_t cursorf;
			cursorf.x = (float)cursor_pt.x;
			cursorf.y = (float)cursor_pt.y;

			HDC hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &current_rect);
			FillRect(h_chdc, &current_rect, h_white_brush);
			draw_textf(h_chdc, 10, 10, "For paint polygon, press right mouse button, for clear polygon, press ESC");
			draw_textf(h_chdc, 10, 30, "For move point, click left mouse button");
			draw_polygon(h_chdc, poly_coords, curr_poly_coord);

			polygon_t polygon;
			polygon.num_of_points = curr_poly_coord;
			polygon.p_points = poly_coords;

			RECT cursor_rect;
			cursor_rect.left = cursor_pt.x - 2;
			cursor_rect.top = cursor_pt.y - 2;
			cursor_rect.right = cursor_pt.x + 2;
			cursor_rect.bottom = cursor_pt.y + 2;
			FillRect(h_chdc, &cursor_rect, (HBRUSH)(DKGRAY_BRUSH + 1));

			BitBlt(hdc, 0, 0, current_rect.right, current_rect.bottom, h_chdc, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			break;
		}

		case WM_MOUSEMOVE: {
			cursor_pt.x = LOWORD(lParam);
			cursor_pt.y = HIWORD(lParam);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}

		case WM_RBUTTONDOWN: {
			poly_coords[curr_poly_coord].x = LOWORD(lParam);
			poly_coords[curr_poly_coord].y = HIWORD(lParam);
			curr_poly_coord = (curr_poly_coord + 1) % MAX_POLYGON_COORDS;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}

		case WM_KEYDOWN:
			if (wParam == 27) {
				curr_poly_coord = 0;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;

		case WM_ERASEBKGND:
			return 1;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ErrorMessage(const char *p_format, ...)
{
	char buffer[512];
	va_list argptr;
	va_start(argptr, p_format);
	vsprintf_s(buffer, sizeof(buffer), p_format, argptr);
	va_end(argptr);

	MessageBoxA(0, buffer, "Error", MB_OK | MB_ICONERROR);
}