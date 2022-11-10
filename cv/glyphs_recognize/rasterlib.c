#include "rasterlib.h"

malloc_func mallocfn = malloc;
calloc_func callocfn = calloc;

int attributes = 0;
//TODO: options stack
//TODO: layers array
//TODO: image processors array

bool      rlib_init(malloc_func p_malloc, calloc_func p_calloc);
void      rlib_shutdown();
bool      rlib_register_image_processor(rlib_image_processor_dt_t *p_imgprocdt, const char *p_formatname);
bool      rlib_unregister_image_processor(const char *p_formatname);

bool      rlib_enable(int opt);
bool      rlib_disable(int opt);
bool      rlib_is_enabled(int opt);
void      rlib_push_option(int opt);
void      rlib_pop_option(int opt);

bool      rlib_export_image(hbitmap_t h_bitmap, const char *p_format, const char *p_filename);
hbitmap_t rlib_load_image(const char *p_image);
hbitmap_t rlib_create_bitmap(int width, int height, int bpp, int format);
hbitmap_t rlib_copy_bitmap(hbitmap_t h_bitmap);
void      rlib_delete_bitmap(hbitmap_t hbitmap);
uchar_t  *rlib_get_bitmap_pixels(hbitmap_t hbitmap);
void      rlib_bitmap_fill_pixel(hbitmap_t hbitmap, int x, int y, color_t color);

hlayer_t  rlib_create_layer(hlayer_t h_parent_layer, int width, int height);
void      rlib_delete_layer(hlayer_t h_layer);
rlibobj_t rlib_select(hlayer_t h_layer, int res_type, rlibobj_t h_object);
rlibobj_t rlib_get_object(hlayer_t h_layer, int res_type);
void      rlib_push(hlayer_t h_layer, int res_type);
void      rlib_pop(hlayer_t h_layer);
bool      rlib_flip(hlayer_t h_layer, int flipop);
bool      rlib_transpose(hlayer_t h_layer, int trpop);
bool      rlib_bitblt(hlayer_t h_dst_layer, int xdst, int ydst, const hlayer_t h_src_layer, int xsrc, int ysrc, int rstrop);

void      rlib_draw_horizontal_line(hlayer_t h_layer, int from_x, int to_x, int y, color_t color);
void      rlib_draw_vertical_line(hlayer_t h_layer, int x, int from_y, int to_y, color_t color);
void      rlib_draw_pixel(hlayer_t h_layer, const point_t *p_pt, color_t color);
void      rlib_draw_line(hlayer_t h_layer, const point_t *p_from, const point_t *p_to);
void      rlib_draw_polyline(hlayer_t h_layer, const point_t *p_points, int pts_count);
void      rlib_draw_rect(hlayer_t h_layer, int x, int y, int width, int height, int fillmode);
void      rlib_draw_polygon(hlayer_t h_layer, const point_t *p_points, int pts_count, int fillmode);
void      rlib_draw_circle(hlayer_t h_layer, int x, int y, int radius, color_t color);

const rlib_dt_t *get_raster_api(int version)
{
	static const rlib_dt_t dt = {
		.rlib_init = rlib_init,
		.rlib_shutdown = rlib_shutdown,
		.rlib_register_image_processor = rlib_register_image_processor,
		.rlib_unregister_image_processor = rlib_unregister_image_processor,

		.rlib_enable = rlib_enable,
		.rlib_disable = rlib_disable,
		.rlib_is_enabled = rlib_is_enabled,
		.rlib_push_option = rlib_push_option,
		.rlib_pop_option = rlib_pop_option,

		.rlib_export_image = rlib_export_image,
		.rlib_load_image = rlib_load_image,
		.rlib_create_bitmap = rlib_create_bitmap,
		.rlib_copy_bitmap = rlib_copy_bitmap,
		.rlib_delete_bitmap = rlib_delete_bitmap,
		.rlib_get_bitmap_pixels = rlib_get_bitmap_pixels,
		.rlib_bitmap_fill_pixel = rlib_bitmap_fill_pixel,

		.rlib_create_layer = rlib_create_layer,
		.rlib_delete_layer = rlib_delete_layer,
		.rlib_select = rlib_select,
		.rlib_get_object = rlib_get_object,
		.rlib_push = rlib_push,
		.rlib_pop = rlib_pop,
		.rlib_flip = rlib_flip,
		.rlib_transpose = rlib_transpose,
		.rlib_bitblt = rlib_bitblt,

		.rlib_draw_horizontal_line = rlib_draw_horizontal_line,
		.rlib_draw_vertical_line = rlib_draw_vertical_line,
		.rlib_draw_pixel = rlib_draw_pixel,
		.rlib_draw_line = rlib_draw_line,
		.rlib_draw_polyline = rlib_draw_polyline,
		.rlib_draw_rect = rlib_draw_rect,
		.rlib_draw_polygon = rlib_draw_polygon,
		.rlib_draw_circle = rlib_draw_circle
	};
	return &dt;
}

bool rlib_init(malloc_func p_malloc, calloc_func p_calloc)
{
}

void rlib_shutdown()
{
}

bool rlib_register_image_processor(rlib_image_processor_dt_t *p_imgprocdt, const char *p_formatname)
{
}

bool rlib_unregister_image_processor(const char *p_formatname)
{
}


bool rlib_enable(int opt)
{
	attributes |= (1 << opt);
}

bool rlib_disable(int opt)
{
	attributes &= ~(1 << opt);
}

bool rlib_is_enabled(int opt)
{
	return (bool)(attributes & (1 << opt));
}

void rlib_push_option(int opt)
{
}

void rlib_pop_option(int opt)
{
}


bool rlib_export_image(hbitmap_t h_bitmap, const char *p_format, const char *p_filename)
{
}

hbitmap_t rlib_load_image(const char *p_image)
{
}

hbitmap_t rlib_create_bitmap(int width, int height, int bpp, int format)
{
}

hbitmap_t rlib_copy_bitmap(hbitmap_t h_bitmap)
{
}

void rlib_delete_bitmap(hbitmap_t hbitmap)
{
}

uchar_t  *rlib_get_bitmap_pixels(hbitmap_t hbitmap)
{
}

void rlib_bitmap_fill_pixel(hbitmap_t hbitmap, int x, int y, color_t color)
{
}


hlayer_t rlib_create_layer(hlayer_t h_parent_layer, int width, int height)
{
}

void rlib_delete_layer(hlayer_t h_layer)
{
}

rlibobj_t rlib_select(hlayer_t h_layer, int res_type, rlibobj_t h_object)
{
}

rlibobj_t rlib_get_object(hlayer_t h_layer, int res_type)
{
}

void rlib_push(hlayer_t h_layer, int res_type)
{
}

void rlib_pop(hlayer_t h_layer)
{
}

bool rlib_flip(hlayer_t h_layer, int flipop)
{
}

bool rlib_transpose(hlayer_t h_layer, int trpop)
{
}

bool rlib_bitblt(hlayer_t h_dst_layer, int xdst, int ydst, const hlayer_t h_src_layer, int xsrc, int ysrc, int rstrop)
{
}


void rlib_draw_horizontal_line(hlayer_t h_layer, int from_x, int to_x, int y, color_t color)
{
}

void rlib_draw_vertical_line(hlayer_t h_layer, int x, int from_y, int to_y, color_t color)
{
}

void rlib_draw_pixel(hlayer_t h_layer, const point_t *p_pt, color_t color)
{
}

void rlib_draw_line(hlayer_t h_layer, const point_t *p_from, const point_t *p_to)
{
}

void rlib_draw_polyline(hlayer_t h_layer, const point_t *p_points, int pts_count)
{
}

void rlib_draw_rect(hlayer_t h_layer, int x, int y, int width, int height, int fillmode)
{
}

void rlib_draw_polygon(hlayer_t h_layer, const point_t *p_points, int pts_count, int fillmode)
{
}

void rlib_draw_circle(hlayer_t h_layer, int x, int y, int radius, color_t color)
{

}