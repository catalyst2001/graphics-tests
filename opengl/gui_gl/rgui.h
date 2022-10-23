#pragma once
#include <windows.h>
#include <gl/GL.h>

#include "rcolor.h"
#include "../../common/glmath.h"

#include <vector>

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

enum rbutton_state : char {
	BUTTON_DOWN = 0,
	BUTTON_UP
};

// 
// Basic ui element interface
// 
class rgui_iface_base
{
protected:
	virtual void draw() = 0;
	virtual void mousemove(int x, int y) = 0;
	virtual void mouseclick(int x, int y, int button, rbutton_state state) = 0;
	virtual void keyinput(wchar_t sym) = 0;
	virtual void keydown(wchar_t sym, rbutton_state state) = 0;
	virtual void mousewheel(int mwdelta) = 0;
	virtual void screen_resize(long width, long height) = 0;
};

class rgui_rect
{
public:
	union {
		struct { long left, top, right, bottom; };
		struct { long x, y, width, height; };
	};

	rgui_rect() : left(0), top(0), right(0), bottom(0) {}
	rgui_rect(long x, long y, long width, long height) { init_from_coord(x, y, width, height); }

	void init_from_coord(long x, long y, long width, long height) {
		left = x;
		top = y;
		right = left + width;
		bottom = top + height;
	}

	bool pt_in_rect(rgui_point &pt) { return ( left < pt.x && top > pt.y && right > pt.x && bottom < pt.y ); }

	long get_width() { return right - left; }
	long get_height() { return bottom - top; }
};

enum rgui_element_general_flags : char {
	UIF_NONE = 0,				   // element no have flags
	UIF_ELEMVIS = (1 << 0),		   // element has visible
	UIF_ELEMEVENTSRECV = (1 << 1)  // element receved events (mouse move, keys down)
};

class rgui_root_dispatcher;

// 
// Basic ui element
// 
class rgui_baseelement : public rgui_iface_base
{
	friend class rgui_root_dispatcher;

	int flags;
	const char *p_name;
	rgui_baseelement *p_parentaddr;
	rgui_rect clip_rect;
	std::vector<rgui_baseelement *> childs;
protected:
	rgui_baseelement() : flags(UIF_ELEMVIS|UIF_ELEMEVENTSRECV), p_name(nullptr), p_parentaddr(nullptr) {}
	rgui_baseelement(rgui_baseelement *p_parent, long x, long y, long width, long height) : flags(UIF_ELEMVIS|UIF_ELEMEVENTSRECV), p_name(nullptr), p_parentaddr(nullptr) {
		clip_rect.init_from_coord(x, y, width, height);
		if(p_parent)
			p_parent->add_child(this);

		p_parentaddr = p_parent;
	}

	void draw() {
	}

	void mousemove(int x, int y) {
	}

	void mouseclick(int x, int y, int button, rbutton_state state) {
	}

	void keyinput(wchar_t sym) {
	}

	void keydown(wchar_t sym, rbutton_state state) {
	}

	void mousewheel(int mwdelta) {
	}

	void screen_resize(long width, long height) {
	}

public:
	void set_flags(int fl) { flags = fl; }
	int  get_flags() { return flags; }
	void set_name(const char *name) { p_name = name; }
	const char *get_name() { return p_name; }

	// 
	// add child to element
	// 
	bool add_child(rgui_baseelement *p_child) {
		for (size_t i = 0; i < childs.size(); i++)
			if (childs[i] == p_child)
				return false;
		
		childs.push_back(p_child);
		return true;
	}

	// 
	// remove child from elmeent
	// 
	bool remove_child(rgui_baseelement *p_child) {
		for (size_t i = 0; i < childs.size(); i++) {
			if (childs[i] == p_child) {
				childs.erase(childs.begin() + i);
				return true;
			}
		}
		return false;
	}

	rgui_baseelement *get_parent() { return p_parentaddr; }

	// TODO: 
	// TODO: 
	// TODO: 
	void move(long x, long y) {
		if (p_parentaddr) {
			//clip_rect.init_from_coord(p_parentaddr->clip_rect.x + x, p_parentaddr->clip_rect.y + y, );
			return;
		}
		clip_rect.x += x;
		clip_rect.y += y;
	}

	void resize(long width, long height) {
		if (p_parentaddr) {
			clip_rect.width = p_parentaddr->clip_rect.width + clip_rect.width + width;
			clip_rect.height = p_parentaddr->clip_rect.height + clip_rect.height + height;
			return;
		}
	}
};

// 
// Basic drawing routines
// 
inline void rgui_draw_rect(int x, int y, int width, int height)
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
// UNUSED
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

// 
// Main UI class
// Route all events to childs
// 
class rgui_root_dispatcher : public rgui_baseelement
{
	rgui_rect root_rect; //screen rect
public:
	rgui_root_dispatcher() {
		set_name("RootDispatcher");
	}
	~rgui_root_dispatcher() {}

	void set_screen_rect(long x, long y, long width, long height) {
		root_rect.init_from_coord(x, y, width, height);
	}

private:
#define IS_DISABLED(elemx) (!elemx || !((elemx)->flags & UIF_ELEMVIS))

	/* inner recursive functions */
	void draw_recursive(rgui_baseelement *p_self);
	void mousemove_recursive(rgui_baseelement *p_self, int x, int y);
	void mouseclick_recursive(rgui_baseelement *p_self, int x, int y, int button, rbutton_state state);
	void keyinput_recursive(rgui_baseelement *p_self, wchar_t sym);
	void keydown_recursive(rgui_baseelement *p_self, wchar_t sym, rbutton_state state);
	void mousewheel_recursive(rgui_baseelement *p_self, int mwdelta);
	void screen_resize_recursive(rgui_baseelement *p_self, long width, long height);

public:
	/* events */
	void draw()                                          { draw_recursive(this); }
	void mousemove(int x, int y)                         { mousemove_recursive(this, x, y); }
	void mouseclick(int x, int y, int button, rbutton_state state) { mouseclick_recursive(this, x, y, button, state); }
	void keyinput(wchar_t sym)                           { keyinput_recursive(this, sym); }
	void keydown(wchar_t sym, rbutton_state state)       { keydown_recursive(this, sym, state); }
	void mousewheel(int mwdelta)                         { mousewheel_recursive(this, mwdelta); }
	void screen_resize(long width, long height)          { screen_resize_recursive(this, width, height); }
};

class rgui_texture_info
{
	rgui_rect rect;
	const char *p_texture_name;
	unsigned int texture_id;
public:
	rgui_texture_info() : p_texture_name(nullptr), texture_id(0) {}
	~rgui_texture_info() {}

	void set_rect(rgui_rect &new_rect)     { rect = new_rect; }
	rgui_rect &get_rect()                  { return rect; }
	void set_name(const char *p_name)      { p_texture_name = p_name; }
	const char *get_name()                 { return p_texture_name; }
	void set_texid(unsigned int new_texid) { texture_id = new_texid; }
	unsigned int get_texid()               { return texture_id; }
};

class rgui_imaterial
{
public:
	virtual bool load_texture(rgui_texture_info *p_dst_texture, const char *p_path) = 0;
	virtual void unload_texture(const rgui_texture_info *p_texture) = 0;
};

#define NOTHING_MACRO(x)
NOTHING_MACRO(sizeof(rgui_texture_info));

class rgui_base_background
{
	rgui_texture_info background_texture;
public:

};

// 
// Label class
// 
class rgui_label : public rgui_baseelement//, public rgui_base_background
{
	char buffer[256];
	void draw();
public:
	rgui_label(rgui_baseelement *p_parent, long x, long y, long width, long height);
	~rgui_label();

	void set_text(const char *p_text);
	const char *get_text();
	void format(const char *p_format, ...);
};
NOTHING_MACRO(sizeof(rgui_label));

// 
// Image panel class
// 
class rgui_imagepanel : public rgui_baseelement
{
	rgui_texture_info *p_texture;

	void draw();
public:
	rgui_imagepanel(rgui_baseelement *p_parent, long x, long y, long width, long height, const rgui_texture_info *p_imgtexture);
	~rgui_imagepanel();

	void set_image(rgui_texture_info *p_tex);
	rgui_texture_info *get_image();
};
NOTHING_MACRO(sizeof(rgui_imagepanel));