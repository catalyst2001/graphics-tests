#include <windows.h>
#include <stdio.h>

#define WINDOW_CLASS "WindowClass"
#define WIDTH 800
#define HEIGHT 600

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct vec2_t {
	float x, y;
};

#define MAX_POLYPOINTS 100
int poly_points_count = 0;
vec2_t polygon_points[MAX_POLYPOINTS];

#define POINT_BOUND_RADIUS 20
POINT cursor;
bool mouse_active_button[2];
HBRUSH h_red_brush;

/*
=================================
 vec2_add

 add two vectors
=================================
*/
void vec2_add(vec2_t *p_dst, const vec2_t *p_veca, const vec2_t *p_vecb)
{
	p_dst->x = p_veca->x + p_vecb->x;
	p_dst->y = p_veca->y + p_vecb->y;
}

/*
=================================
 vec2_divs

 divide vector by scalar
=================================
*/
void vec2_divs(vec2_t *p_dst, vec2_t *p_vec, float scalar)
{
	p_dst->x = p_vec->x / scalar;
	p_dst->y = p_vec->y / scalar;
}

void compute_centroid(vec2_t *p_dst, const vec2_t *p_polypoints, int num_of_points)
{
	vec2_t sum = { 0.f, 0.f };
	for (int i = 0; i < num_of_points; i++)
		vec2_add(&sum, &sum, &p_polypoints[i]);

	vec2_divs(&sum, &sum, (float)num_of_points);
	*p_dst = sum;
}

bool point_in_circle(vec2_t *p_circle_center, const vec2_t *p_point, float radius)
{
	vec2_t circle;
	circle.x = p_circle_center->x - p_point->x;
	circle.y = p_circle_center->y - p_point->y;
	return ((circle.x * circle.x + circle.y * circle.y) <= (radius * radius)); //x^2 + y^2 <= R^2
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

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = WINDOW_CLASS;
	if (!RegisterClassExA(&wcex)) {
		MessageBoxA(0, "Failed to register window class!", "Error", MB_OK | MB_ICONERROR);
		return TRUE;
	}

	HWND hWnd = CreateWindowExA(0, wcex.lpszClassName, "Polygon Centroid Compute", WS_OVERLAPPEDWINDOW, 
		(GetSystemMetrics(SM_CXSCREEN) / 2) - (WIDTH / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (HEIGHT / 2),
		WIDTH, HEIGHT,
		NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return TRUE;

	h_red_brush = CreateSolidBrush(RGB(255, 0, 0));

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

  //  case WM_COMMAND: {
  //      int wmId = LOWORD(wParam);
  //      switch (wmId)
  //      {
  //      default:
  //          return DefWindowProc(hWnd, message, wParam, lParam);
  //      }
		//break;
  //  }

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

	case WM_ERASEBKGND:
		return 1;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}