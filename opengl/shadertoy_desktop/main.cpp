#include <Windows.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include "windows_controls.h"

#define WINDOW_CLASS "WindowClass"

HINSTANCE g_instance;
HWND h_wnd;

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

	ControlsSetFont((HFONT)GetStockObject(ANSI_VAR_FONT));



	ShowWindow(h_wnd, SW_SHOW);
	UpdateWindow(h_wnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			break;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

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