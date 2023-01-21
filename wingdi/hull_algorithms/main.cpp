#include <windows.h>
#include <stdio.h>
#include "math.h"

/*
==========================
 Window classes definition

 MAIN_WINDOW_CLASS - main window class
 SOURCE_POLY_CLASS - source polygon window (polygon edition window)
 MIXED_POLY_CLASS - viewing the polygon resulting from the violation of the order of points
 HULL_POLY_CLASS - polygon obtained as a result of point sorting
==========================
*/
#define MAIN_WINDOW_CLASS "MainWindow"
#define SOURCE_POLY_CLASS "PolyWindow"

/* controls IDs */
#define IDC_HULL_ALGHORITM 100

#define WIDTH 800
#define HEIGHT 600
#define BOTTOM_PADDING 40
#define BETWEEN_PADDING 2
#define BUTTON_WIDTH 20

HWND h_main_window;

union {
	struct {
		HWND h_source_poly_window;
		HWND h_mixed_poly_window;
		HWND h_hull_poly_window;
	};
	HWND handles[3];
} plywindows;

LRESULT CALLBACK wndproc_main(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK wndproc_polywindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void recomute_positions()
{
	HWND h_button;
	RECT window_client, poly_client;
	SIZE poly_window_size;
	GetClientRect(h_main_window, &window_client);
	h_button = GetDlgItem(h_main_window, IDC_HULL_ALGHORITM);
	poly_window_size.cy = window_client.bottom - BOTTOM_PADDING;
	poly_window_size.cx = (window_client.right / 3) - (BETWEEN_PADDING * 2); //3 poly windows

	// source poly edit window
	for (int i = 0; i < 3; i++) {
		poly_client.left = poly_window_size.cx * i + BETWEEN_PADDING;
		poly_client.top = window_client.top - BETWEEN_PADDING;
		poly_client.right = poly_client.left + poly_window_size.cx;
		poly_client.bottom = poly_window_size.cy - BETWEEN_PADDING;
		MoveWindow(plywindows.handles[i], poly_client.left, poly_client.top, poly_client.right, poly_client.bottom, TRUE);
	}

	// move button
	MoveWindow(h_button, BETWEEN_PADDING, window_client.bottom - BETWEEN_PADDING - BUTTON_WIDTH, 50, BUTTON_WIDTH, TRUE);
}

#define MAX_POLYPOINTS 100
int poly_points_count = 0;
vec2_t polygon_points[MAX_POLYPOINTS];

#define POINT_BOUND_RADIUS 20
POINT cursor;
bool mouse_active_button[2];
HBRUSH h_red_brush;

template<typename _Type>
void swap(_Type *p_ea, _Type *p_eb)
{
	_Type temp = *p_ea;
	*p_ea = *p_eb;
	*p_eb = temp;
}

void draw_point(HDC hdc, int x, int y, int pt_size, HBRUSH brush)
{
	RECT pt_rect;
	pt_rect.left = x - pt_size;
	pt_rect.top = y - pt_size;
	pt_rect.right = x + pt_size;
	pt_rect.bottom = y + pt_size;
	FillRect(hdc, &pt_rect, brush);
}

void polygon_draw(HDC hdc)
{
	POINT spt;
	HBRUSH pt_brush;
	if (poly_points_count > 0) {
		pt_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		draw_point(hdc, polygon_points[0].x, polygon_points[0].y, 2, pt_brush);
		MoveToEx(hdc, polygon_points[0].x, polygon_points[0].y, &spt);

		for (int i = 1; i < poly_points_count; i++) {
			LineTo(hdc, polygon_points[i].x, polygon_points[i].y);
			draw_point(hdc, polygon_points[i].x, polygon_points[i].y, 2, pt_brush);
		}

		LineTo(hdc, polygon_points[0].x, polygon_points[0].y);
	}
}

void polygon_add_point(LPPOINT ppt)
{
	vec2_t pt;
	pt.x = (float)ppt->x;
	pt.y = (float)ppt->y;
	if (poly_points_count >= MAX_POLYPOINTS) {
		MessageBoxA(0, "Points limit exceeded!", "Warning!", 0);
		return;
	}
	polygon_points[poly_points_count] = pt;
	poly_points_count++;
}

#define polygon_remove_point() if(poly_points_count > 0) poly_points_count--;

char format_buffer[1024];

const char *format(const char *p_format, ...)
{
	va_list argptr;
	va_start(argptr, p_format);
	vsprintf_s(format_buffer, sizeof(format_buffer), p_format, argptr);
	va_end(argptr);
	return (const char *)format_buffer;
}

#define va(x, ...) format(x, __VA_ARGS__)

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	/* register main window class */
	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpfnWndProc = wndproc_polywindow;
	wcex.lpszClassName = MAIN_WINDOW_CLASS;
	if (!RegisterClassExA(&wcex)) {
		MessageBoxA(0, va("Failed to register main window class!\r\nGetLastError() = %d", GetLastError()), "Error", MB_OK | MB_ICONERROR);
		return TRUE;
	}

	/* register polygon windows class */
	wcex.lpfnWndProc = wndproc_main;
	wcex.lpszClassName = SOURCE_POLY_CLASS;
	if (!RegisterClassExA(&wcex)) {
		MessageBoxA(0, va("Failed to register shape window class!\r\nGetLastError() = %d", GetLastError()), "Error", MB_OK | MB_ICONERROR);
		return TRUE;
	}

	h_main_window = CreateWindowExA(0, wcex.lpszClassName, "Convex/Concave 2D hull alghoritms", WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) / 2) - (WIDTH / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (HEIGHT / 2),
		WIDTH, HEIGHT,
		NULL, NULL, hInstance, NULL);

	if (!h_main_window) {
		MessageBoxA(0, va("Failed to create main window\r\nGetLastError() = %d", GetLastError()), "Error", MB_OK | MB_ICONERROR);
		return TRUE;
	}

	if (!CreateWindowExA(0, "button", "Concave/Convex", WS_CHILD | WS_VISIBLE, 0, 0, 1, 1, h_main_window, (HMENU)IDC_HULL_ALGHORITM, NULL, NULL)) {
		MessageBoxA(0, va("Failed to create button\r\nGetLastError() = %d", GetLastError()), "Error", MB_OK | MB_ICONERROR);
		return TRUE;
	}

	/* create polygon windows */
	plywindows.h_source_poly_window = CreateWindowExA(WS_EX_CLIENTEDGE, SOURCE_POLY_CLASS, "", WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 1, 1, h_main_window, (HMENU)0, NULL, NULL);
	plywindows.h_mixed_poly_window = CreateWindowExA(WS_EX_CLIENTEDGE, SOURCE_POLY_CLASS, "", WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 1, 1, h_main_window, (HMENU)0, NULL, NULL);
	plywindows.h_hull_poly_window = CreateWindowExA(WS_EX_CLIENTEDGE, SOURCE_POLY_CLASS, "", WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 1, 1, h_main_window, (HMENU)0, NULL, NULL);
	if (!plywindows.h_source_poly_window || !plywindows.h_mixed_poly_window || !plywindows.h_hull_poly_window) {
		MessageBoxA(0, va("Failed to create one of poly windows\r\nGetLastError() = %d", GetLastError()), "Error", MB_OK | MB_ICONERROR);
		return TRUE;
	}

	h_red_brush = CreateSolidBrush(RGB(255, 0, 0));

	recomute_positions();
	ShowWindow(h_main_window, SW_SHOW);
	UpdateWindow(h_main_window);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(h_red_brush);
	return FALSE;
}

void DrawTextf(HDC hdc, int x, int y, const char *p_text, ...)
{
	char buffer[1024];
	va_list argptr;
	va_start(argptr, p_text);
	vsprintf_s(buffer, sizeof(buffer), p_text, argptr);
	va_end(argptr);

	TextOutA(hdc, x, y, buffer, strlen(buffer));
}

LRESULT CALLBACK wndproc_main(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: {

		break;
	}

	case WM_SIZE: {
		recomute_positions();
		break;
	}

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcA(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK wndproc_polywindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	switch (message)
	{
	case WM_CREATE:
		HDC hdc;
		hdc = GetDC(hWnd);
		SelectObject(hdc, GetStockObject(ANSI_VAR_FONT));
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_MOUSEMOVE:
		cursor.x = LOWORD(lParam);
		cursor.y = HIWORD(lParam);

		vec2_t fcursor;
		fcursor.x = (float)cursor.x;
		fcursor.y = (float)cursor.y;
		if (mouse_active_button[0] && poly_points_count > 0) {
			for (int i = 0; i < poly_points_count; i++) {
				if (point_in_circle(&polygon_points[i], &fcursor, POINT_BOUND_RADIUS)) {
					polygon_points[i] = fcursor;
					goto __invalidate;
				}
			}
		}

	__invalidate:
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONDOWN:
		mouse_active_button[0] = true;
		cursor.x = LOWORD(lParam);
		cursor.y = HIWORD(lParam);
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONUP:
		mouse_active_button[0] = false;
		break;

	case WM_RBUTTONDOWN:
		mouse_active_button[1] = true;
		cursor.x = LOWORD(lParam);
		cursor.y = HIWORD(lParam);
		polygon_add_point(&cursor);
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_RBUTTONUP:
		mouse_active_button[1] = false;
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_BACK:
			polygon_remove_point();
			break;
		}
		break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		GetClientRect(hWnd, &rect);
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC chdc = CreateCompatibleDC(hdc);
		HBITMAP h_bitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		SelectObject(chdc, h_bitmap);
		FillRect(chdc, &rect, (HBRUSH)(WHITE_BRUSH));

		DrawTextf(chdc, cursor.x, cursor.y, "( %d %d )", cursor.x, cursor.y);
		polygon_draw(chdc);

		// prevent division by zero in compute_centroid function (poly_points_count)
		if (poly_points_count > 0) {
			vec2_t centroid;
			compute_centroid(&centroid, polygon_points, poly_points_count);
			draw_point(chdc, (int)centroid.x, (int)centroid.y, 5, h_red_brush);
		}
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, chdc, 0, 0, SRCCOPY);
		DeleteDC(chdc);
		DeleteObject(h_bitmap);
		EndPaint(hWnd, &ps);
		break;
	}

	//case WM_ERASEBKGND:
	//	return 1;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}