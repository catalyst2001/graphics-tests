#include "main.h"

HINSTANCE g_instance;
HWND h_wnd;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = _T("WindowClass");
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassEx(&wcex)) {
		MessageBox(HWND_DESKTOP, _T("Couldn't create window class"), _T("Critical error"), MB_OK | MB_ICONERROR);
		return 1;
	}

	g_instance = hInstance;
	INT width = 800;
	INT height = 600;
	INT xpos = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
	INT ypos = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);
	h_wnd = CreateWindowEx(0, wcex.lpszClassName, _T("Scan filter"), WS_OVERLAPPEDWINDOW, xpos, ypos, width, height, NULL, NULL, hInstance, NULL);
	if (!h_wnd) {
		MessageBox(HWND_DESKTOP, _T("Couldn't create window"), _T("Critical error"), MB_OK | MB_ICONERROR);
		return 2;
	}
	ShowWindow(h_wnd, nCmdShow);
	UpdateWindow(h_wnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	static POINT pt;
	static POINT start_pt = {0, 0};
	static BOOL is_move = FALSE;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
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
			HDC chdc = CreateCompatibleDC(hdc);
			HBITMAP bitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(chdc, bitmap);
			FillRect(chdc, &rect, GetStockObject(WHITE_BRUSH));
			
			HBRUSH oldbrush = SelectObject(chdc, GetStockObject(GRAY_BRUSH));
			Rectangle(chdc, start_pt.x, start_pt.y, start_pt.x + 100, start_pt.y + 100);
			SelectObject(chdc, oldbrush);
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, chdc, 0, 0, SRCCOPY);
			DeleteDC(chdc);
			DeleteObject(bitmap);
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
