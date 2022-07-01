#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

HINSTANCE g_instance;
HWND h_wnd;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

HBITMAP image;

class DrawingLayer
{
	HDC memdc;
	SIZE bitmapsize;
	HBITMAP membitmap;
public:
	DrawingLayer() {}
	~DrawingLayer() {}

	BOOL Create(HDC hdc, LONG width, LONG height) {
		memdc = CreateCompatibleDC(hdc);
		membitmap = CreateCompatibleBitmap(hdc, width, height);
		SelectObject(memdc, membitmap);
		bitmapsize.cx = width;
		bitmapsize.cy = height;
		return TRUE;
	}

	BOOL Delete() {
		DeleteDC(memdc);
		DeleteObject(membitmap);
		return TRUE;
	}

	VOID Draw(HDC hdc, LONG zoom, PPOINT pos) {
		if (zoom) {
			StretchBlt(hdc, pos->x, pos->y, bitmapsize.cx + zoom, bitmapsize.cy + zoom, memdc, 0, 0, bitmapsize.cx, bitmapsize.cy, SRCCOPY);
			return;
		}
		BitBlt(hdc, pos->x, pos->y, bitmapsize.cx + zoom, bitmapsize.cy + zoom, memdc, 0, 0, SRCCOPY);
	}

	VOID Draw(HDC hdc, LONG zoom, LONG x, LONG y) {
		POINT pt;
		pt.x = x;
		pt.y = y;
		Draw(hdc, zoom, &pt);
	}

	operator HDC() { return memdc; }
};

class MemoryBitmap
{
	PBYTE ppixels;
	SIZE bitmapsize;
	LONG bytes_per_pixel;
public:
	MemoryBitmap() : ppixels(0) {}
	~MemoryBitmap() {}

	BOOL Create(HBITMAP hbitmap) {

		BITMAP pbi;
		GetObject(hbitmap, sizeof(pbi), &pbi);
		bitmapsize.cx = pbi.bmWidth;
		bitmapsize.cy = pbi.bmHeight;
		bytes_per_pixel = pbi.bmBitsPixel / 8;
		LONG total_bytes = pbi.bmWidth * pbi.bmHeight * bytes_per_pixel;
		ppixels = (PBYTE)malloc((size_t)total_bytes);
		assert(ppixels);
		GetBitmapBits(hbitmap, total_bytes, ppixels);

		LONG j = 0;
		if (pbi.bmWidthBytes) {
			for (LONG y = 0; y < pbi.bmHeight; y++) {
				PBYTE rowpixels = (PBYTE)pbi.bmBits + y * pbi.bmWidthBytes;
				for (LONG x = 0; x < pbi.bmWidth; x++, rowpixels += bytes_per_pixel) {
					PBYTE ppixel = &ppixels[(y * pbi.bmWidth + x) * bytes_per_pixel];

					// copy pixel data
					for (LONG k = 0; k < bytes_per_pixel; k++) {
						ppixel[k] = rowpixels[k];
					}
				}
			}
		}
		else {
			//TODO:  
		}
		return TRUE;
	}

	//Flip RGB (or BGR) image Left/Right.
	//P is an array of <height> pointers.
	//P[0] points first pixel of first row.
	//P[1] points first pixel of second row.
	//P[height-1] points first pixel of last row.
	void FlipHorizontal()
	{
		//Allocate sketch buffer for storing single flipped row.
		PBYTE R = (PBYTE)malloc(bitmapsize.cx * bytes_per_pixel);

		//Stuck here don't know how flip those pixels 
		for (int i = 0; i < bitmapsize.cy; i++)
		{
			PBYTE I0 = &ppixels[(i * bitmapsize.cx) * bytes_per_pixel]; //Points first pixel in source image

			//j is destination index (index of rgb triple in source image I).
			for (int j = 0; j < bitmapsize.cx; j++)
			{
				//Iterate source row from end of row to beginning of row.
				R[(j * bytes_per_pixel) + 0] = I0[((bitmapsize.cx - j - 1) * bytes_per_pixel) + 0];
				R[(j * bytes_per_pixel) + 1] = I0[((bitmapsize.cx - j - 1) * bytes_per_pixel) + 1];
				R[(j * bytes_per_pixel) + 2] = I0[((bitmapsize.cx - j - 1) * bytes_per_pixel) + 2];
			}

			//Copy flipped row back to image P.
			memcpy(I0, R, bitmapsize.cx * bytes_per_pixel);
		}
		free(R);
	}

	LONG GetBytesPerPixel() { return bytes_per_pixel * 8; }
	LONG GetBitsPerPixel() { return bytes_per_pixel; }
	LONG GetWidth() { return bitmapsize.cx; }
	LONG GetHeight() { return bitmapsize.cy; }

	PBYTE GetPixel(LONG x, LONG y) {
		if (x < 0 || y < 0 || x > bitmapsize.cx || y > bitmapsize.cy)
			return NULL;

		return &ppixels[(y * bitmapsize.cx + x) * bytes_per_pixel];
	}

	VOID Delete() {
		if (ppixels)
			free(ppixels);
	}

};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEXA);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = "WindowClass";
	if (!RegisterClassExA(&wcex)) {
		MessageBoxA(HWND_DESKTOP, "Couldn't create window class", "Critical error", MB_OK | MB_ICONERROR);
		return 1;
	}

	g_instance = hInstance;
	INT width = 800;
	INT height = 600;
	INT xpos = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
	INT ypos = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);
	h_wnd = CreateWindowEx(0, wcex.lpszClassName, "Scan filter", WS_OVERLAPPEDWINDOW, xpos, ypos, width, height, NULL, NULL, hInstance, NULL);
	if (!h_wnd) {
		MessageBox(HWND_DESKTOP, "Couldn't create window", "Critical error", MB_OK | MB_ICONERROR);
		return 2;
	}
	ShowWindow(h_wnd, nCmdShow);
	UpdateWindow(h_wnd);

	image = (HBITMAP)LoadImageA(NULL, "1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!image) {
		MessageBoxA(0, "Faield to load image", 0, 0);
		return 11;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(image);
	return (int)msg.wParam;
}

bool find_edge(PBITMAP pbitmap, HBITMAP hbitmap, PPOINT ppoints, COLORREF background_color, COLORREF edge_color)
{
	LONG bytes_per_pixel = pbitmap->bmBitsPixel / 8;
	LONG total_bytes = pbitmap->bmWidth * pbitmap->bmHeight * bytes_per_pixel;
	PBYTE ppixels = (PBYTE)malloc((size_t)total_bytes);
	GetBitmapBits(hbitmap, total_bytes, ppixels);

	//for(LONG x = 0; x < )

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static LONG zoom = 0;
	static RECT rect;
	static POINT pt;
	static POINT start_pt = { 0, 0 };
	static BOOL is_move = FALSE;
	switch (message)
	{
	case WM_CREATE: {
		AllocConsole();
		freopen("conout$", "w", stdout);
		printf("Console intialized\n");
		//HBITMAP bitmap = (HBITMAP)LoadImageA(0, "test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		//MemoryBitmap membitmap;
		//membitmap.Create(bitmap);
		//membitmap.FlipHorizontal();
		//for (LONG x = 0; x < membitmap.GetWidth(); x++) {
		//	for (LONG y = 0; y < membitmap.GetHeight(); y++) {
		//		PBYTE pixel = membitmap.GetPixel(x, y);
		//		printf("( %d %d %d ) ", pixel[0], pixel[1], pixel[2]);
		//	}
		//	printf("\n");
		//}
		break;
	}



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

		DrawingLayer display_layer;
		display_layer.Create(hdc, rect.right, rect.bottom);
		FillRect(display_layer, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

		BITMAP bitmap;
		GetObject(image, sizeof(BITMAP), &bitmap);
		DrawingLayer view_layer;
		view_layer.Create(display_layer, bitmap.bmWidth + zoom, bitmap.bmHeight + zoom);
		
		char text[128];
		sprintf_s(text, "WidthBytes: %d\n", bitmap.bmWidthBytes);
		OutputDebugStringA(text);

		HDC bitmapdc = CreateCompatibleDC(view_layer);
		SelectObject(bitmapdc, image);
		//StretchBlt(chdc, start_pt.x, start_pt.y, bitmap.bmWidth + zoom, bitmap.bmHeight + zoom, bitmapdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		BitBlt(view_layer, 0, 0, bitmap.bmWidth, bitmap.bmHeight, bitmapdc, 0, 0, SRCCOPY);
		DeleteDC(bitmapdc);

		view_layer.Draw(display_layer, 0, &start_pt);
		view_layer.Delete();

		display_layer.Draw(hdc, 0, 0, 0);
		display_layer.Delete();
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
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

	case WM_MOUSEWHEEL: {
		SHORT delta = GET_WHEEL_DELTA_WPARAM(wParam);

		char gag[512];
		sprintf_s(gag, "WM_MOUSEWHEEL: %d\n", delta);
		OutputDebugStringA(gag);

		zoom += delta;
		if (zoom < 0)
			zoom = 0;

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}


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
