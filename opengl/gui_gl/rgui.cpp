#include "rgui.h"

void rgui_root_dispatcher::draw_recursive(rgui_baseelement *p_self) {
	for (size_t i = 0; i < p_self->childs.size(); i++) {
		rgui_baseelement *p_elem = p_self->childs[i];
		if (p_elem && (p_elem->flags & UIF_ELEMVIS)) {
			p_elem->draw();
			draw_recursive(p_self->childs[i]);
		}
	}
}

void rgui_root_dispatcher::mousemove_recursive(rgui_baseelement *p_self, int x, int y)
{
	for (size_t i = 0; i < p_self->childs.size(); i++) {
		rgui_baseelement *p_elem = p_self->childs[i];
		if (p_elem && (p_elem->flags & UIF_ELEMEVENTSRECV)) {
			p_elem->mousemove(x, y);
			mousemove_recursive(p_self->childs[i], x, y);
		}
	}
}

void rgui_root_dispatcher::mouseclick_recursive(rgui_baseelement *p_self, int x, int y, int button, rbutton_state state)
{
	for (size_t i = 0; i < p_self->childs.size(); i++) {
		rgui_baseelement *p_elem = p_self->childs[i];
		if (p_elem && (p_elem->flags & UIF_ELEMEVENTSRECV)) {
			p_elem->mouseclick(x, y, button, state);
			mouseclick_recursive(p_self->childs[i], x, y, button, state);
		}
	}
}

void rgui_root_dispatcher::keyinput_recursive(rgui_baseelement *p_self, wchar_t sym)
{
	for (size_t i = 0; i < p_self->childs.size(); i++) {
		rgui_baseelement *p_elem = p_self->childs[i];
		if (p_elem && (p_elem->flags & UIF_ELEMEVENTSRECV)) {
			p_elem->keyinput(sym);
			keyinput_recursive(p_self->childs[i], sym);
		}
	}
}

void rgui_root_dispatcher::keydown_recursive(rgui_baseelement *p_self, wchar_t sym, rbutton_state state)
{
	for (size_t i = 0; i < p_self->childs.size(); i++) {
		rgui_baseelement *p_elem = p_self->childs[i];
		if (p_elem && (p_elem->flags & UIF_ELEMEVENTSRECV)) {
			p_elem->keydown(sym, state);
			keydown_recursive(p_self->childs[i], sym, state);
		}
	}
}

void rgui_root_dispatcher::mousewheel_recursive(rgui_baseelement *p_self, int mwdelta)
{
	for (size_t i = 0; i < p_self->childs.size(); i++) {
		rgui_baseelement *p_elem = p_self->childs[i];
		if (p_elem && (p_elem->flags & UIF_ELEMEVENTSRECV)) {
			p_elem->mousewheel(mwdelta);
			mousewheel_recursive(p_self->childs[i], mwdelta);
		}
	}
}

void rgui_root_dispatcher::screen_resize_recursive(rgui_baseelement *p_self, long width, long height)
{
	for (size_t i = 0; i < p_self->childs.size(); i++) {
		rgui_baseelement *p_elem = p_self->childs[i];
		if (p_elem && (p_elem->flags & UIF_ELEMEVENTSRECV)) {
			p_elem->screen_resize(width, height);
			screen_resize_recursive(p_self->childs[i], width, height);
		}
	}
}

// --------------------------------------------------------------------------------------------------
void rgui_label::draw()
{
}

rgui_label::rgui_label(rgui_baseelement *p_parent, long x, long y, long width, long height) : rgui_baseelement(p_parent, x, y, width, height)
{
	set_name("Label");
}

rgui_label::~rgui_label()
{
}

void rgui_label::set_text(const char *p_text)
{
	strcpy_s(buffer, sizeof(buffer), p_text);
}

const char *rgui_label::get_text()
{
	return (const char *)buffer;
}

void rgui_label::format(const char *p_format, ...)
{
	va_list argptr;
	va_start(argptr, p_format);
	vsprintf_s(buffer, sizeof(buffer), p_format, argptr);
	va_end(argptr);
}

// --------------------------------------------------------------------------------------------------
void rgui_imagepanel::draw()
{
}

rgui_imagepanel::rgui_imagepanel(rgui_baseelement *p_parent, long x, long y, long width, long height, const rgui_texture_info *p_imgtexture) : rgui_baseelement(p_parent, x, y, width, height), p_texture(nullptr)
{
	set_name("Label");
	p_texture = (rgui_texture_info *)p_imgtexture;
}

rgui_imagepanel::~rgui_imagepanel()
{
}

void rgui_imagepanel::set_image(rgui_texture_info *p_tex)
{
	p_texture = p_tex;
}

rgui_texture_info *rgui_imagepanel::get_image()
{
	return p_texture;
}
