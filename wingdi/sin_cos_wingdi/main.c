#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>

HINSTANCE g_instance;
HWND h_wnd;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

HWND h_angle_button, h_angle_input;
HPEN h_red_pen, h_blue_pen;

#define IDC_APPLYANGLE 101

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = _T("SinCosTest");
	if (!RegisterClassEx(&wcex)) {
		MessageBox(HWND_DESKTOP, _T("Couldn't create window class"), _T("Critical error"), MB_OK | MB_ICONERROR);
		return 1;
	}

	g_instance = hInstance;
	INT width = 400;
	INT height = 400;
	INT xpos = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
	INT ypos = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);
	h_wnd = CreateWindowEx(0, wcex.lpszClassName, _T("Sin Cos visualize"), WS_OVERLAPPEDWINDOW, xpos, ypos, width, height, NULL, NULL, hInstance, NULL);
	if (!h_wnd) {
		MessageBox(HWND_DESKTOP, _T("Couldn't create window"), _T("Critical error"), MB_OK | MB_ICONERROR);
		return 2;
	}

	//h_angle_input = CreateWindowExA(WS_EX_CLIENTEDGE, "edit", "", WS_CHILD|WS_VISIBLE, 10, 20, 40, 20, h_wnd, (HMENU)IDC_APPLYANGLE, 0, 0);
	//h_angle_button = CreateWindowExA(0, "button", "Задать угол", WS_CHILD | WS_VISIBLE, 40 + 5 + 20, 20, 60, 20, h_wnd, (HMENU)IDC_APPLYANGLE, 0, 0);
	//SendMessageA(h_angle_button, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), (LPARAM)TRUE);
	h_red_pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	h_blue_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	ShowWindow(h_wnd, nCmdShow);
	UpdateWindow(h_wnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

// -----------
// DrawLine
// отрисовать линию с одной точки до другой
// -----------
void DrawLine(HDC dc, int fx, int fy, int tx, int ty)
{
	MoveToEx(dc, fx, fy, NULL);
	LineTo(dc, tx, ty);
}

void DrawCircleCentred(HDC hdc, int x, int y, int radius)
{
	RECT circlerect;
	circlerect.left = x - radius;
	circlerect.top = y - radius;
	circlerect.right = x + radius;
	circlerect.bottom = y + radius;
	Ellipse(hdc, circlerect.left, circlerect.top, circlerect.right, circlerect.bottom);
}

#define LINE_LENGTH 200

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	static POINT pt;
	static POINT start_pt = { 0, 0 };
	static BOOL is_move = FALSE;
	static POINT graphic_offset;
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case 1:
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		graphic_offset.x = 0;
		graphic_offset.y = 0;

		HDC chdc = CreateCompatibleDC(hdc);
		HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		SelectObject(chdc, bitmap);
		FillRect(chdc, &rect, GetStockObject(WHITE_BRUSH));

		HBRUSH oldbrush = SelectObject(chdc, GetStockObject(GRAY_BRUSH));
		HPEN hOldPen = SelectObject(chdc, h_blue_pen);

		DrawLine(chdc, graphic_offset.x, graphic_offset.y, graphic_offset.x, graphic_offset.y + LINE_LENGTH);
		TextOutA(chdc, graphic_offset.x, graphic_offset.y + LINE_LENGTH, "Y", 1);

		DrawLine(chdc, graphic_offset.x, graphic_offset.y, graphic_offset.x + LINE_LENGTH, graphic_offset.y);
		TextOutA(chdc, graphic_offset.x + LINE_LENGTH, graphic_offset.y, "X", 1);
		SelectObject(chdc, hOldPen);

		hOldPen = SelectObject(chdc, h_red_pen);
		DrawLine(chdc, graphic_offset.x, graphic_offset.y, start_pt.x, start_pt.y);
		DrawCircleCentred(chdc, start_pt.x, start_pt.y, 10);
		SelectObject(chdc, hOldPen);

		SelectObject(chdc, oldbrush);
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, chdc, 0, 0, SRCCOPY);
		DeleteDC(chdc);
		DeleteObject(bitmap);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		DeleteObject(h_red_pen);
		DeleteObject(h_blue_pen);
		break;

	case WM_LBUTTONDOWN:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		is_move = TRUE;
		break;

	case WM_MOUSEMOVE: {
		POINT mousept;
		if (is_move) {
			mousept.x = LOWORD(lParam);
			mousept.y = HIWORD(lParam);
			start_pt.x += mousept.x - pt.x;
			start_pt.y += mousept.y - pt.y;
			pt = mousept;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}

	case WM_LBUTTONUP:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		is_move = FALSE;
		break;

	case WM_RBUTTONDOWN:
		start_pt.x = LINE_LENGTH;
		start_pt.y = LINE_LENGTH;
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
