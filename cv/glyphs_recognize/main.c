#include <Windows.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include "rasterlib.h"

int min_pixel = 20;

#define WINDOW_CLASS "WindowClass"

HINSTANCE g_instance;
HWND h_wnd;

void ErrorMessage(const char *p_format, ...);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

rlib_dt_t *p_reapi;

HBITMAP h_bitmap;

size_t bmp_pixels_total_size(const PBITMAP p_bitmap)
{
	return (size_t)(p_bitmap->bmWidth * p_bitmap->bmHeight * (p_bitmap->bmBitsPixel / 8));
}

void aligned_bmp_to_unaligned(unsigned char *p_dstpixels, const PBITMAP p_bitmap)
{
	int bytes_per_pixel = p_bitmap->bmBitsPixel / 8;
	for (int y = 0, y2 = p_bitmap->bmHeight - 1; y < p_bitmap->bmHeight; y++, y2--) {
		PBYTE pixels = (PBYTE)p_bitmap->bmBits + y2 * p_bitmap->bmWidthBytes;
		for (int x = 0; x < p_bitmap->bmWidth; x++, pixels += bytes_per_pixel) {
			memcpy(&p_dstpixels[((y * p_bitmap->bmWidth) + x) * bytes_per_pixel], pixels, bytes_per_pixel);
		}
	}
}

int get_avg_color(PBYTE p_pixels, int bpp, int width, int height)
{
	PBYTE p_pixel;
	long long colors_summ[3] = { 0, 0, 0 };
	int bytes_per_pixel = bpp / 8;
	long long pixels_count = height * height;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			p_pixel = &p_pixels[((y * width) + x) * bytes_per_pixel];
			colors_summ[0] += p_pixel[0];
			colors_summ[1] += p_pixel[1];
			colors_summ[2] += p_pixel[2];
		}
	}
	colors_summ[0] /= pixels_count;
	colors_summ[1] /= pixels_count;
	colors_summ[2] /= pixels_count;
	return RGB3(RLIB_CLAMP(colors_summ[0], 0, 255), RLIB_CLAMP(colors_summ[1], 0, 255), RLIB_CLAMP(colors_summ[2], 0, 255));
}

void apply_negative(PBYTE p_pixels, int bpp, int width, int height)
{
	PBYTE p_pixel;
	int bytes_per_pixel = bpp / 8;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			p_pixel = &p_pixels[((y * width) + x) * bytes_per_pixel];
			p_pixel[0] = RLIB_CLAMP(255 - p_pixel[0], 0, 255);
			p_pixel[1] = RLIB_CLAMP(255 - p_pixel[1], 0, 255);
			p_pixel[2] = RLIB_CLAMP(255 - p_pixel[2], 0, 255);
		}
	}
}

void apply_gray(PBYTE p_pixels, int bpp, int width, int height)
{
	PBYTE p_pixel;
	int bytes_per_pixel = bpp / 8;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			p_pixel = &p_pixels[((y * width) + x) * bytes_per_pixel];
			int pix = (p_pixel[0] + p_pixel[1] + p_pixel[2]) / 3;
			p_pixel[0] = RLIB_CLAMP(pix, 0, 255);
			p_pixel[1] = RLIB_CLAMP(pix, 0, 255);
			p_pixel[2] = RLIB_CLAMP(pix, 0, 255);
		}
	}
}

void compute_boundbox(rect_t *p_dstrect, PBYTE p_pixels, int bpp, int width, int height, color_t color)
{
	if (bpp % 2)
		return;

	if (bpp < 24 || bpp > 32)
		return;

	PBYTE p_pixel;
	int bytes_per_pixel = bpp / 8;
	int x, y;
#define GET_PIXEL(x, y) (&p_pixels[((y * width) + x) * bytes_per_pixel])
#define IS_DARKER(p_rgb_pixel) (p_rgb_pixel[0] < color.r && p_rgb_pixel[1] < color.g && p_rgb_pixel[2] < color.b)

	//left top
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			p_pixel = GET_PIXEL(x, y);
			if (IS_DARKER(p_pixel)) {
				p_dstrect->left = x;
				p_dstrect->top = y;
				break;
			}
		}
	}

	// right top
	//for (y = 0; y < height; y++) {
	//	for (x = width - 1; x > 0; x--) {
	//		p_pixel = GET_PIXEL(x, y);
	//		if (IS_DARKER(p_pixel)) {
	//			points[1].x = x;
	//			points[1].y = y;
	//			break;
	//		}
	//	}
	//}

	// left bottom
	//for (y = height - 1; y > 0; y--) {
	//	for (x = 0; x < width; x++) {
	//		p_pixel = GET_PIXEL(x, y);
	//		if (IS_DARKER(p_pixel)) {
	//			points[2].x = x;
	//			points[2].y = y;
	//			break;
	//		}
	//	}
	//}

	// right bottom
	for (x = width - 1; x > 0; x--) {
		for (y = height - 1; y > 0; y--) {
			p_pixel = GET_PIXEL(x, y);
			if (IS_DARKER(p_pixel)) {
				p_dstrect->right = x;
				p_dstrect->bottom = y;
				break;
			}
		}
	}
#undef GET_PIXEL
#undef IS_DARKER
}

void test1()
{
	AllocConsole();
	freopen("conout$", "w", stdout);

	BITMAP bmpinf;
	GetObjectA(h_bitmap, sizeof(bmpinf), &bmpinf);
	size_t size = bmp_pixels_total_size(&bmpinf);
	uchar_t *p_pixels = (uchar_t *)calloc(size, 1);
	aligned_bmp_to_unaligned(p_pixels, &bmpinf);

	PBYTE p_pixel;
	long long colors_summ[3] = { 0, 0, 0 };
	int bytes_per_pixel = bmpinf.bmBitsPixel / 8;
	for (int y = 0; y < bmpinf.bmHeight; y++) {
		for (int x = 0; x < bmpinf.bmWidth; x++) {
			p_pixel = &p_pixels[((y * bmpinf.bmWidth) + x) * bytes_per_pixel];
			printf("%c", (p_pixel[0] < 50 && p_pixel[1] < 50 && p_pixel[2] < 50) ? '&' : ' ');
		}
		printf("\n");
	}
	printf("\n");
	free(p_pixels);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
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
	p_reapi = get_raster_api(0);
	p_reapi->rlib_init(0, 0);

	//hlayer_t h_layer = p_reapi->rlib_create_layer(NULL, 400, 400);

	if (!(h_bitmap = LoadImageA(0, "text.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION)))
		MessageBoxA(0, 0, 0, 0);

	test1();

	ShowWindow(h_wnd, SW_SHOW);
	UpdateWindow(h_wnd);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0)) {
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
			RECT rect;
			GetClientRect(hWnd, &rect);
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			HDC chdc = CreateCompatibleDC(hdc);
			HBITMAP h_drawbitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(chdc, h_drawbitmap);

			BITMAP bmpinf;
			GetObjectA(h_bitmap, sizeof(bmpinf), &bmpinf);
			HDC imagedc = CreateCompatibleDC(hdc);
			SelectObject(imagedc, h_bitmap);
			//BitBlt(chdc, 0, 0, bmpinf.bmWidth, bmpinf.bmHeight, imagedc, 0, 0, SRCCOPY);

			size_t total_bytes = bmp_pixels_total_size(&bmpinf);
			uchar_t *p_dst_pixels = (uchar_t *)calloc(total_bytes, 1);
			if (!p_dst_pixels)
				MessageBoxA(0, "Out of memory", 0, 0);

			aligned_bmp_to_unaligned(p_dst_pixels, &bmpinf);
			int clr = get_avg_color(p_dst_pixels, bmpinf.bmBitsPixel, bmpinf.bmWidth, bmpinf.bmHeight);
			//char buffer[512];
			//sprintf_s(buffer, sizeof(buffer), "avg color ( %d %d %d )", GETR(clr), GETG(clr), GETB(clr));
			//MessageBoxA(0, buffer, 0, 0);

			//RECT brushrect = { 300, 300, 300 + 50, 300 + 50 };
			//HBRUSH brush = CreateSolidBrush(clr);
			//FillRect(chdc, &brushrect, brush);
			//DeleteObject(brush);

			rect_t bounds;
			color_t color;
			color.intclr = clr;
			compute_boundbox(&bounds, p_dst_pixels, bmpinf.bmBitsPixel, bmpinf.bmWidth, bmpinf.bmHeight, color);
			HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			HBRUSH oldbrush = (HBRUSH)SelectObject(chdc, nullbrush);

			//apply_negative(p_dst_pixels, bmpinf.bmBitsPixel, bmpinf.bmWidth, bmpinf.bmHeight);

			PBYTE p_pixel;
			int bytes_per_pixel = bmpinf.bmBitsPixel / 8;
			for (int y = 0; y < bmpinf.bmHeight; y++) {
				for (int x = 0; x < bmpinf.bmWidth; x++) {
					p_pixel = &p_dst_pixels[((y * bmpinf.bmWidth) + x) * bytes_per_pixel];
					SetPixel(chdc, x, y, RGB(p_pixel[0], p_pixel[1], p_pixel[2]));
				}
			}

			//SetBitmapBits(h_drawbitmap, total_bytes, p_dst_pixels);

			MoveToEx(chdc, bounds.left, bounds.top, NULL);
			LineTo(chdc, bounds.right, bounds.bottom);

			Rectangle(chdc, bounds.left, bounds.top, bounds.right, bounds.bottom);

			//Rectangle(chdc, 5, 5, 10, 10);

			SelectObject(chdc, oldbrush);

			free(p_dst_pixels);

			BitBlt(hdc, 0, 0, rect.right, rect.bottom, chdc, 0, 0, SRCCOPY);
			DeleteDC(imagedc);
			DeleteDC(chdc);
			EndPaint(hWnd, &ps);
			break;
		}

		case WM_KEYDOWN:
			switch (wParam) {
			case VK_DOWN:
				if (min_pixel > 0)
					min_pixel -= 5;

				printf("MinPixel: %d\n", min_pixel);
				InvalidateRect(hWnd, 0, TRUE);
				break;

			case VK_UP:
				min_pixel += 5;
				printf("MinPixel: %d\n", min_pixel);
				InvalidateRect(hWnd, 0, TRUE);
				break;
			}
			break;

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