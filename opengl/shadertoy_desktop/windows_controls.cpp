#include "windows_controls.h"

HFONT g_font;

HFONT ControlsSetFont(HFONT newfont)
{
	HFONT oldfont = g_font;
	if(newfont)
		g_font = newfont;

	return oldfont;
}

HWND AddChildWindow(HWND parent, const char * wndclass, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle, DWORD dwExStyle)
{
	return CreateWindowExA(dwExStyle, wndclass, "", WS_VISIBLE|WS_CHILD|dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
}

#define SET_FONT(handle) SendMessageA(handle, WM_SETFONT, (WPARAM)g_font, (LPARAM)NULL)

HWND AddButton(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	HWND hbutton = CreateWindowExA(0, "button", name, WS_VISIBLE | WS_CHILD | dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	SET_FONT(hbutton);
	return hbutton;
}

HWND AddImageButton(HWND parent, HBITMAP icon, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	HWND hbutton = CreateWindowExA(0, "button", "", WS_VISIBLE|WS_CHILD|BS_BITMAP|dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	SendMessageA(hbutton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)icon);
	return hbutton;
}


HWND AddToggleButton(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	HWND togglebutton = CreateWindowExA(0, "button", name, WS_VISIBLE|WS_CHILD|BS_3STATE|dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	SET_FONT(togglebutton);
	return togglebutton;
}

HWND AddCheckBox(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	HWND hchk = CreateWindowExA(0, "button", name, WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	SET_FONT(hchk);
	return hchk;
}

HWND AddRadioButton(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	HWND hradio = CreateWindowExA(0, "button", name, WS_VISIBLE|WS_CHILD|BS_AUTORADIOBUTTON|dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	SET_FONT(hradio);
	return hradio;
}

HWND AddGroupBox(HWND parent, const char *name, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	HWND hgroup = CreateWindowExA(0, "button", name, WS_VISIBLE|WS_CHILD|BS_GROUPBOX|dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	SET_FONT(hgroup);
	return hgroup;
}

HWND AddTrackbar(HWND parent, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	return CreateWindowExA(0, TRACKBAR_CLASSA, "", WS_VISIBLE | WS_CHILD | BS_GROUPBOX | dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
}

HWND AddTrackbar(HWND parent, INT x, INT y, INT width, INT height, INT id, INT _min, INT _max, DWORD dwStyle)
{
	HWND htrack = AddTrackbar(parent, x, y, width, height, id, dwStyle);
	SendMessageA(htrack, TBM_SETRANGEMIN, (WPARAM)FALSE, _min);
	SendMessageA(htrack, TBM_SETRANGEMAX, (WPARAM)TRUE, _max);
	return htrack;
}

HWND AddComboBox(HWND parent, INT x, INT y, INT width, INT height, INT id, DWORD dwStyle)
{
	HWND hcombobox = CreateWindowExA(0, WC_COMBOBOXA, "", WS_VISIBLE | WS_CHILD | WS_TABSTOP | /*WS_VSCROLL | CBS_DROPDOWNLIST | */dwStyle, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	SET_FONT(hcombobox);
	return hcombobox;
}

HWND AddListbox(HWND window, int id, int posx, int posy, int width, int height, bool bmultiselect)
{
	long flags = (WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_NOTIFY | WS_VSCROLL | WS_HSCROLL | WS_BORDER);
	if (bmultiselect)
		flags |= (LBS_MULTIPLESEL | LBS_EXTENDEDSEL);

	HWND listbox = CreateWindowExA(WS_EX_CLIENTEDGE, "listbox", "", flags, posx, posy, width, height, window, (HMENU)id, 0, 0);
	SET_FONT(listbox);
	return listbox;
}

HWND AddLabel(HWND window, const char *name, int id, int pox, int posy, int width, int height)
{
	HWND label = CreateWindowExA(WS_EX_TRANSPARENT, "static", "", WS_VISIBLE | WS_CHILD, pox, posy, width, height, window, (HMENU)id, 0, 0);
	SET_FONT(label);
	SetWindowTextA(label, name);
	return label;
}

HWND AddEditbox(HWND window, const char *deftext, int flags, int id, int posx, int posy, int width, int height, int textlimit, bool breadonly, bool bpassword)
{
	//ES_READONLY, ES_PASSWORD
	long flag = (WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL) | flags;
	if (breadonly)
		flag |= ES_READONLY;
	if (bpassword)
		flag |= ES_PASSWORD;

	if (!deftext)
		deftext = "";

	HWND editbox = CreateWindowExA(WS_EX_CLIENTEDGE, "edit", deftext, flag, posx, posy, width, height, window, (HMENU)id, 0, 0);
	SET_FONT(editbox);
	SendMessageA(editbox, EM_LIMITTEXT, (WPARAM)textlimit, 0L);
	return editbox;
}

int AddSceneViewport(viewport_t *pviewport, const char *windowclass, HWND parent, INT x, INT y, INT width, INT height, INT bitsPerPixel, INT depthBits)
{
	pviewport->width = width;
	pviewport->height = height;
	pviewport->colorbits = bitsPerPixel;
	pviewport->depthbits = depthBits;
	if(!(pviewport->window = AddChildWindow(parent, windowclass, x, y, width, height, 0, NULL, WS_EX_CLIENTEDGE)))
		return VIEWPORT_FAILED_CREATE_WINDOW;

	if (!(pviewport->devctx = GetDC(pviewport->window)))
		return VIEWPORT_FAILED_GET_DEVICE_CONTEXT;

	//MSDN: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-choosepixelformat
	int pixfmt;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		pviewport->colorbits,    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		pviewport->depthbits,		// 32-bit z-buffer      
		0,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};

	if (!(pixfmt = ChoosePixelFormat(pviewport->devctx, &pfd)))
		return VIEWPORT_FAILED_CHOOSE_PIXEL_FORMAT;

	if (!SetPixelFormat(pviewport->devctx, pixfmt, &pfd))
		return VIEWPORT_FAILED_SET_PIXEL_FORMAT;

	if (!(pviewport->rndctx = wglCreateContext(pviewport->devctx)))
		return VIEWPORT_FAILED_CREATE_RENDERING_CONTEXT;

	wglMakeCurrent(pviewport->devctx, pviewport->rndctx);
	return VIEWPORT_OK;
}

void DeleteViewport(viewport_t *pviewport)
{
	wglDeleteContext(pviewport->rndctx);
	ReleaseDC(pviewport->window, pviewport->devctx);
	DestroyWindow(pviewport->window);
}


#undef SET_FONT
