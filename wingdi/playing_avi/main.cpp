#include <Windows.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <Vfw.h>
#pragma comment(lib, "Vfw32.lib")

#define WINDOW_CLASS "WindowClass"

HINSTANCE g_instance;
HWND h_wnd;

void ErrorMessage(const char *p_format, ...);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

PAVISTREAM p_avi;
AVISTREAMINFOA avi_stream_info;
PGETFRAME p_getframe;
HBITMAP h_bitmap;
PBYTE p_pixels;
HDRAWDIB h_drawdib;
BITMAPINFOHEADER bitmap_header;

long max_frames;
long milliseconds_per_frame;

#define IDT_REDRAW 100

int main()
{
	HINSTANCE hInstance = GetModuleHandleA(0);
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
	h_wnd = CreateWindowExA(0, wcex.lpszClassName, "Playing AVI animation", WS_OVERLAPPEDWINDOW, xpos, ypos, width, height, NULL, NULL, hInstance, NULL);
	if (!h_wnd) {
		ErrorMessage("Couldn't create window");
		return 2;
	}

	// open avi file
	CHAR errbuf[512];
	AVIFileInit();
	if (AVIStreamOpenFromFileA(&p_avi, "Valve.avi", streamtypeVIDEO, 0, OF_READ, NULL) != 0) {
		sprintf_s(errbuf, sizeof(errbuf), "Failed to open AVI!\r\nError: %d", GetLastError());
		MessageBoxA(0, errbuf, "Error open AVI", MB_OK|MB_ICONERROR);
		return 1;
	}

	AVIStreamInfoA(p_avi, &avi_stream_info, sizeof(avi_stream_info));
	printf("------ AVIStreamInfoA -------\n");
	printf(
		"dwFormatChangeCount: %d\n"
		"dwInitialFrames: %d\n"
		"dwLength: %d\n"
		"dwQuality: %d\n"
		"dwRate: %d\n"
		"dwSampleSize: %d\n"
		"dwScale: %d\n"
		"dwStart: %d\n"
		"dwSuggestedBufferSize: %d\n"
		"szName: %s\n",
		avi_stream_info.dwFormatChangeCount,
		avi_stream_info.dwInitialFrames,
		avi_stream_info.dwLength,
		avi_stream_info.dwQuality,
		avi_stream_info.dwRate,
		avi_stream_info.dwSampleSize,
		avi_stream_info.dwScale,
		avi_stream_info.dwStart,
		avi_stream_info.dwSuggestedBufferSize,
		avi_stream_info.szName
	);
	printf("-----------------------------\n");

	max_frames = AVIStreamLength(p_avi);
	if (!max_frames) {
		MessageBoxA(0, "0 frames in avi", "Empty AVI", MB_OK|MB_ICONERROR);
		return 1;
	}
	milliseconds_per_frame = AVIStreamSampleToTime(p_avi, max_frames) / max_frames;

	bitmap_header.biSize = sizeof(bitmap_header);
	bitmap_header.biPlanes = 1;
	bitmap_header.biBitCount = 24;
	bitmap_header.biWidth = avi_stream_info.rcFrame.right - avi_stream_info.rcFrame.left;
	bitmap_header.biHeight = avi_stream_info.rcFrame.bottom - avi_stream_info.rcFrame.top;

	xpos = (GetSystemMetrics(SM_CXSCREEN) / 2) - (bitmap_header.biWidth / 2);
	ypos = (GetSystemMetrics(SM_CYSCREEN) / 2) - (bitmap_header.biHeight / 2);
	MoveWindow(h_wnd, xpos, ypos, bitmap_header.biWidth, bitmap_header.biHeight, TRUE);

	HDC h_dc = GetDC(h_wnd);
	h_bitmap = CreateDIBSection(h_dc, (BITMAPINFO *)&bitmap_header, DIB_RGB_COLORS, (void **)&p_pixels, NULL, NULL);
	h_drawdib = DrawDibOpen();
	SelectObject(h_dc, h_bitmap);
	ReleaseDC(h_wnd, h_dc);

	p_getframe = AVIStreamGetFrameOpen(p_avi, NULL);
	if (!p_getframe) {
		MessageBoxA(0, "Failed to open AVI frame", "AVI frame error", MB_OK | MB_ICONERROR);
		return 1;
	}

	SetTimer(h_wnd, IDT_REDRAW, milliseconds_per_frame, NULL);

	ShowWindow(h_wnd, SW_SHOW);
	UpdateWindow(h_wnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
	DrawDibClose(h_drawdib);
	DeleteObject(h_bitmap);
	AVIStreamRelease(p_avi);
	AVIFileExit();
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_TIMER: {
			if (wParam == IDT_REDRAW) {
				InvalidateRect(hWnd, NULL, FALSE);
				printf("IDT_REDRAW\n");
			}

			break;
		}

		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			break;
		}

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			static long position = 0;
			LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(p_getframe, position);
			char *pdata = (char *)lpbi + lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD); // Pointer To Data Returned By AVIStreamGetFrame
			DrawDibDraw(h_drawdib, hdc, 0, 0, bitmap_header.biWidth, bitmap_header.biHeight, lpbi, pdata, 0, 0, bitmap_header.biWidth, bitmap_header.biHeight, 0);
			//BitBlt(hdc, 0, 0, bitmap_header.biWidth, bitmap_header.biHeight, chdc, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			position = (position + 1) % avi_stream_info.dwLength;
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