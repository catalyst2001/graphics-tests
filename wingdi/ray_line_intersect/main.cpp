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

int ray_coords_idx = 0;
int line_coords_idx = 0;
vec2_t ray_coords[2];
vec2_t line_coords[2];

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

			/* draw other info */
			sprintf_s(buffer, sizeof(buffer), "Mouse coords ( %d %d )  Window size: ( %d %d )", cursor_pt.x, cursor_pt.y, current_rect.right, current_rect.bottom);
			TextOutA(h_chdc, 10, 30, buffer, strlen(buffer));
			

			
			/* draw ray */
			vec2_t dst = { 1000.f, 0.f };
			vec2_add(&dst, &cursorf, &dst);
			
			sprintf_s(buffer, sizeof(buffer), "Ray ( %f %f )", dst.x, dst.y);
			TextOutA(h_chdc, cursor_pt.x, cursor_pt.y, buffer, strlen(buffer));

			h_oldpen = SelectObject(h_chdc, h_pen_ray);
			draw_line(h_chdc, cursor_pt.x, cursor_pt.y, (int)dst.x, (int)dst.y);
			SelectObject(h_chdc, h_oldpen);

			/* draw line */
			h_oldpen = SelectObject(h_chdc, h_pen_poly);
			draw_line(h_chdc, (int)line_coords[0].x, (int)line_coords[0].y, (int)line_coords[1].x, (int)line_coords[1].y);
			sprintf_s(buffer, sizeof(buffer), "Line (%f %f) (%f %f)", line_coords[0].x, line_coords[0].y, line_coords[1].x, line_coords[1].y);
			TextOutA(h_chdc, (int)line_coords[0].x, (int)line_coords[0].y, buffer, strlen(buffer));
			SelectObject(h_chdc, h_oldpen);

			//vec2_t diff;
			//vec2_sub(&diff, &line_coords[1], &line_coords[0]);

			static const char *p_text[] = { "No intersect", "INTERSECT!!!" };
			char *p_str = (char *)p_text[line_line_intersection2(NULL, cursorf, dst, line_coords[1], line_coords[0])];
			TextOutA(h_chdc, 10, 10, p_str, strlen(p_str));



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
			line_coords[line_coords_idx].x = LOWORD(lParam);
			line_coords[line_coords_idx].y = HIWORD(lParam);
			line_coords_idx = (line_coords_idx + 1) % 2;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}

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