#pragma once
#include <windows.h>
#include <gl/GL.h>

#include "rcolor.h"
#include "../../common/glmath.h"

//  
// Basic point class
//  
class rgui_point
{
public:
	int x, y;

	rgui_point() {}
	rgui_point(int _x, int _y) : x(_x), y(_y) {}
	~rgui_point() {}
};

// 
// Basic uv texture coords class
// 
class rgui_uv
{
public:
	float u, v;
	rgui_uv() {}
	rgui_uv(float _u, float _v) : u(_u), v(_v) {}
	~rgui_uv() {}
};

// 
// Basic shape vertex class
// 
class rgui_vertex
{
public:
	rgui_point pt;
	rgui_uv uv;
	rutl::rcolor4 clr;
};

// 
// Basic ui element interface
// 
class rgui_iface_base
{
	virtual void draw() = 0;
	virtual void mousemove(int x, int y) = 0;
	virtual void mouseclick(int x, int y, int button, int state) = 0;
	virtual void keyinput(wchar_t sym) = 0;
	virtual void mousewheel(int mwdelta) = 0;
	virtual bool set_hidestate(bool bhide) = 0;
};

class rgui_rect
{
public:


};

// 
// Basic ui element
// 
class rgui_baseelement : public rgui_iface_base
{
	int flags;
	const char *p_name;
public:
	void draw()
	{
	}

	void mousemove(int x, int y)
	{
	}

	void mouseclick(int x, int y, int button, int state)
	{
	}

	void keyinput(wchar_t sym)
	{
	}

	void mousewheel(int mwdelta)
	{
	}

	bool set_hidestate(bool bhide)
	{
	}

	void set_flags(int fl) { flags = fl; }
	int  get_flags() { return flags; }
	void set_name(const char *name) { p_name = name; }
	const char *get_name() { return p_name; }
};

// 
// Basic drawing routines
// 
inline void rgui_DrawRect(int x, int y, int width, int height)
{
	int vertices[] = {
		x, y,
		x + width, y,
		x + width, y + height,
		x, y + height
	};
	glVertexPointer(2, GL_INT, 0, vertices);
	glDrawArrays(GL_QUADS, 0, 4);
}

// ------------------------------
// rgui_baseframe
// 
// ------------------------------
class rgui_baseframe
{
	rgui_point pts[4];
	rgui_uv uvs[4];
	rutl::rcolor4 colors[4];
public:
	rgui_baseframe() {}
	~rgui_baseframe() {}

	void draw_baseframe();


};