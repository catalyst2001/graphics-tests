#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <commdlg.h>
#include <stdio.h>

#pragma comment(lib, "comctl32.lib")

HFONT ControlsSetFont(HFONT newfont);

/**
* Child window
*/
HWND AddChildWindow(HWND parent, const char *wndclass, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0, DWORD dwExStyle = 0);

/**
* Buttons
*/
HWND AddButton(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);
HWND AddImageButton(HWND parent, HBITMAP icon, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);
HWND AddToggleButton(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);
HWND AddCheckBox(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);
HWND AddRadioButton(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);
HWND AddGroupBox(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);

/**
* Trackbar
*/
HWND AddTrackbar(HWND parent, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);
HWND AddTrackbar(HWND parent, INT x, INT y, INT width, INT height, INT id, INT _min, INT _max, DWORD dwStyle = 0);
#define TrackbarSetPos(handle, newpos) SendMessageA(handle, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)newpos);
#define TrackbarGetPos(handle) SendMessageA(handle, TBM_GETPOS, (WPARAM)NULL, (LPARAM)NULL);

/**
* Combobox
*/
HWND AddComboBox(HWND parent, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle = 0);
#define ComboBox_AddString(handle, string) SendMessageA(handle, CB_ADDSTRING, (WPARAM)NULL, (LPARAM)string) //return - index of 0
#define ComboBox_DeleteString(handle, index) SendMessageA(handle, CB_DELETESTRING, (WPARAM)index, (LPARAM)NULL)
#define ComboBox_GetCount(handle) SendMessageA(handle, CB_GETCOUNT, (WPARAM)NULL, (LPARAM)NULL)
#define ComboBox_GetCurrent(handle) SendMessageA(handle, CB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL)

/**
* Menus
*/
#define MenuAddItem(hmenu, name, id) AppendMenuA(hmenu, MF_STRING, id, name)

/***
* Listbox
*/
HWND AddListbox(HWND window, int id, int posx, int posy, int width, int height, bool bmultiselect = false);

#define LbAddItem(hlistbox, itemname) (int)SendMessageA(hlistbox, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)itemname)
#define LbSelectItem(hlistbox, itemidx) (int)SendMessageA(hlistbox, LB_SETCURSEL, (WPARAM)itemidx, 0L)
#define LbUnselectItem(hlb, idx) SendMessageA(hlb, LB_SETSEL, (WPARAM)FALSE, (LPARAM)(UINT)idx)
#define LbSetItemData(hlb, idx, data) SendMessageA(hlb, LB_SETITEMDATA, (WPARAM)idx, (LPARAM)data)
#define LbGetItemData(hlb, idx) SendMessageA(hlb, LB_GETITEMDATA, (WPARAM)idx, 0L);
#define LbGetItemText(hlb, idx, buffer) SendMessageA(hlb, LB_GETTEXT, (WPARAM)idx, (LPARAM)buffer)
#define LbItemIsSelected(hlb, idx) !!SendMessageA(hlb, LB_GETSEL, (WPARAM)idx, 0L)

/**
* Label
*/
HWND AddLabel(HWND window, const char *name, int id, int pox, int posy, int width, int height);

HWND AddEditbox(HWND window, const char *deftext, int flags, int id, int posx, int posy, int width, int height, int textlimit = 255, bool breadonly = false, bool bpassword = false);
#define EditboxSetText(hedit, text) SetWindowTextA(hedit, text)
#define EditboxGetText(hedit, buffer, maxlen) GetWindowTextA(hedit, buffer, maxlen)

// Open dialog for select colors
//	HWND parent - parent window
//	color_t *color - pointer to color var
static void OpenSelectColorDialog(HWND parent, BYTE *pr, BYTE *pg, BYTE *pb)
{
	CHOOSECOLOR cc;
	static COLORREF custColors[16];
	BYTE rr = 255;
	BYTE gg = 255;
	BYTE bb = 255;
	memset(&cc, 0, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = parent;
	cc.rgbResult = RGB(rr, gg, bb);
	cc.lpCustColors = custColors;
	cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	if (ChooseColorA(&cc)) {
		*pr = (int)GetRValue(cc.rgbResult);
		*pg = (int)GetGValue(cc.rgbResult);
		*pb = (int)GetBValue(cc.rgbResult);
	}
}

// Создаёт диалог выбора файла для открытия
//	 _In_ HWND window - родительское окно
//	 _Out_ char *path - буфер для сохранения выбранного пути
//	 _In_ const char *initialdir - путь отображаемый при старте диалога
//	 _In_ const char *filter - расширения подходящих для выбора файлов
static BOOL OpenFileDialog(_In_ HWND window, _Out_ char *path, _In_ const char *initialdir, _In_ const char *filter)
{
	OPENFILENAMEA ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = window;
	ofn.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = path;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrInitialDir = initialdir;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	return GetOpenFileNameA(&ofn);
}

// Открыть диалог сохранения файла
//	 _In_ HWND window - родительское окно
//	 _Out_ char *path - буфер для сохранения выбранного пути
//	 _In_ const char *initialdir - путь отображаемый при старте диалога
//	 _In_ const char *filter - расширения подходящих для сохранения файлов
static BOOL SaveFileDialog(_In_ HWND window, _Out_ char *path, _In_ const char *initialdir, _In_ const char *filter)
{
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = window;
	ofn.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = path;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrInitialDir = initialdir;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	return GetSaveFileNameA(&ofn);
}

/**
* OpenGL scene viewport
*/
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

typedef struct viewport_s {
	HWND window;
	HDC devctx;
	HGLRC rndctx;
	INT width;
	INT height;
	INT colorbits;
	INT depthbits;
} viewport_t;

enum {
	VIEWPORT_OK = 0,
	VIEWPORT_FAILED_CREATE_WINDOW,
	VIEWPORT_FAILED_GET_DEVICE_CONTEXT,
	VIEWPORT_FAILED_CREATE_RENDERING_CONTEXT,
	VIEWPORT_FAILED_CHOOSE_PIXEL_FORMAT,
	VIEWPORT_FAILED_SET_PIXEL_FORMAT,
};

int AddSceneViewport(viewport_t *pviewport, const char *windowclass, HWND parent, INT x, INT y, INT width, INT height, INT bitsPerPixel, INT depthBits);
void DeleteViewport(viewport_t *pviewport);

/**
* Toolbar
*/
class CToolbar
{
public:
	CToolbar() {}
	~CToolbar() {}

	HIMAGELIST g_hImageList = NULL;

	//https://docs.microsoft.com/ru-ru/windows/win32/controls/create-toolbars
	//https://docs.microsoft.com/ru-ru/windows/win32/controls/create-vertical-toolbars
	HWND CreateSimpleToolbar(HWND hWndParent, HINSTANCE hInstance)
	{
		const int ImageListID = 0;
		const int numButtons = 3;
		const int bitmapSize = 16;

		const DWORD buttonStyles = BTNS_AUTOSIZE;

		// Create the toolbar.
		HWND hWndToolbar = CreateWindowExA(0, TOOLBARCLASSNAMEA, NULL, WS_CHILD|TBSTYLE_WRAPABLE/*| TBSTYLE_LIST*/, 0, 0, 800, 40, hWndParent, NULL, hInstance, NULL);

		if (hWndToolbar == NULL)
			return NULL;


		// Initialize button info.
		// IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.
		TBBUTTON tbButtons[] =
		{
			{ MAKELONG(STD_FILENEW,  ImageListID), 5, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"New" },
			{ MAKELONG(STD_FILEOPEN, ImageListID), 6, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"Open"},
			{ MAKELONG(STD_FILESAVE, ImageListID), 7, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"Save"},
			{ MAKELONG(STD_UNDO, ImageListID), 8, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"Undo"},
			{ MAKELONG(STD_REDOW, ImageListID), 9, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"Redo"},
			{ MAKELONG(STD_PROPERTIES, ImageListID), 10, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"Settings"},
			{ MAKELONG(STD_FIND, ImageListID), 11, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"Find"},
			{ MAKELONG(STD_HELP, ImageListID), 12, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)"Help"},
		};

		// Create the image list.
		g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
			ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
			sizeof(tbButtons) / sizeof(TBBUTTON), 0);

		SendMessageA(hWndToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID, (LPARAM)g_hImageList); // Set the image list.
		SendMessageA(hWndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL); // Load the button images.

		// Add buttons.
		SendMessageA(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessageA(hWndToolbar, TB_ADDBUTTONS, (WPARAM)sizeof(tbButtons) / sizeof(TBBUTTON), (LPARAM)&tbButtons);

		// Resize the toolbar, and then show it.
		SendMessageA(hWndToolbar, TB_AUTOSIZE, 0, 0);
		ShowWindow(hWndToolbar, TRUE);

		return hWndToolbar;
	}
};