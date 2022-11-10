#pragma once
/**
* Rasterizer Libary
* 
* Author: Deryabin K.
* Date: 11.11.2022
*/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define RLIBNO(x)

typedef unsigned char uchar_t;

typedef void *(__cdecl *malloc_func)(size_t size);
typedef void *(__cdecl *calloc_func)(size_t count, size_t size);

typedef void *rlibobj_t;
typedef rlibobj_t hbitmap_t;
typedef rlibobj_t hlayer_t;

typedef struct color_s {
	union {
		struct { unsigned char r, g, b, a; };
		unsigned char bytes[4];
		int intclr;
	};
} color_t, const cclr_t;
RLIBNO(sizeof(color_t));

typedef struct point_s {
	int x, y;
} point_t;

typedef struct bitmap_s {
	int pixels_format;
	int bits_per_pixel;
	int bytes_per_pixel;
	int width;
	int height;
	size_t total_bytes;
	uchar_t *p_data;
} bitmap_t;

enum BITMAP_FLIP_OPERATION {
	BITMAP_FLIP_VERTICAL = 0,
	BITMAP_FLIP_HORIZONTAL
};

enum BITMAP_RASTER_OPERATION {
	BITMAP_ROP_SRCCOPY
};

bool bitmap_create(bitmap_t *p_dstbmp, int format, int bpp, int width, int height);
bool bitmap_flip(const bitmap_t *p_srcbmp, int flipop);
void bitmap_free(const bitmap_t *p_srcbmp);
bool bitmap_bitblt(bitmap_t *p_dstbmp, int xdst, int ydst, const bitmap_t *p_srcbmp, int xsrc, int ysrc, int rastrop);

typedef struct layer_s {
	bitmap_t *p_bitmap;
} layer_t;

hbitmap_t rlib_create_bitmap(int width, int height, int bpp, int format);
void rlib_delete_bitmap(hbitmap_t hbitmap);

hlayer_t rlib_create_layer(hlayer_t h_parent_layer);
void rlib_delete_layer(hlayer_t h_layer);
bool rlib_draw_pixel(hlayer_t h_layer, const point_t *p_pt, const color_t *p_srcclr);
bool rlib_draw_line(hlayer_t h_layer, const point_t *p_from, const point_t *p_to);
bool rlib_draw_polyline(hlayer_t h_layer, const point_t *p_points, int pts_count);
bool rlib_draw_rect(hlayer_t h_layer, int x, int y, int width, int height, int fillmode);
bool rlib_draw_polygon(hlayer_t h_layer, const point_t *p_points, int pts_count, int fillmode);

// tools flags
#define TOOL_PEN (1 << 0)
#define TOOL_BRUSH (1 << 1)

#define RGB3(r, g, b)    (r | (g << 8) | (b << 16))
#define RGB4(r, g, b, a) (r | (g << 8) | (b << 16) | (a << 24))
#define GETR(clr)        (clr & 0xff)
#define GETG(clr)        ((clr >> 8) & 0xff)
#define GETB(clr)        ((clr >> 16) & 0xff)
#define GETA(clr)        ((clr >> 24) & 0xff)

typedef struct rlib_image_processor_dt_s {
	hbitmap_t (*rlib_image_import_routine)(const char *p_path);
	bool      (*rlib_image_export_routine)(hbitmap_t h_bitmap, const char *p_path);
} rlib_image_processor_dt_t;

//built-in image processors
#define BMP_IMAGE "MicrosoftBitMap"
#define TGA_IMAGE "TrueVisionTGA"
#define PCX_IMAGE "PCExchange"
#define DDS_IMAGE "DirectDrawSurface"

#define RLIB_MULTISAMPLING 0

typedef struct rlib_dt_s {
	bool      (*rlib_init)(malloc_func p_malloc, calloc_func p_calloc);
	void      (*rlib_shutdown)();
	bool      (*rlib_register_image_processor)(rlib_image_processor_dt_t *p_imgprocdt, const char *p_formatname);
	bool      (*rlib_unregister_image_processor)(const char *p_formatname);

	bool      (*rlib_enable)(int opt);
	bool      (*rlib_disable)(int opt);
	bool      (*rlib_is_enabled)(int opt);
	void      (*rlib_push_option)(int opt);
	void      (*rlib_pop_option)(int opt);

	bool      (*rlib_export_image)(hbitmap_t h_bitmap, const char *p_format, const char *p_filename);
	hbitmap_t (*rlib_load_image)(const char *p_image);
	hbitmap_t (*rlib_create_bitmap)(int width, int height, int bpp, int format);
	hbitmap_t (*rlib_copy_bitmap)(hbitmap_t h_bitmap);
	void      (*rlib_delete_bitmap)(hbitmap_t hbitmap);
	uchar_t  *(*rlib_get_bitmap_pixels)(hbitmap_t hbitmap);
	void      (*rlib_bitmap_fill_pixel)(hbitmap_t hbitmap, int x, int y, color_t color);

	hlayer_t  (*rlib_create_layer)(hlayer_t h_parent_layer, int width, int height);
	void      (*rlib_delete_layer)(hlayer_t h_layer);
	rlibobj_t (*rlib_select)(hlayer_t h_layer, int res_type, rlibobj_t h_object);
	rlibobj_t (*rlib_get_object)(hlayer_t h_layer, int res_type);
	void      (*rlib_push)(hlayer_t h_layer, int res_type);
	void      (*rlib_pop)(hlayer_t h_layer);
	bool      (*rlib_flip)(hlayer_t h_layer, int flipop);
	bool      (*rlib_transpose)(hlayer_t h_layer, int trpop);
	bool      (*rlib_bitblt)(hlayer_t h_dst_layer, int xdst, int ydst, const hlayer_t h_src_layer, int xsrc, int ysrc, int rstrop);

	void      (*rlib_draw_horizontal_line)(hlayer_t h_layer, int from_x, int to_x, int y, color_t color);
	void      (*rlib_draw_vertical_line)(hlayer_t h_layer, int x, int from_y, int to_y, color_t color);
	void      (*rlib_draw_pixel)(hlayer_t h_layer, const point_t *p_pt, color_t color);
	void      (*rlib_draw_line)(hlayer_t h_layer, const point_t *p_from, const point_t *p_to);
	void      (*rlib_draw_polyline)(hlayer_t h_layer, const point_t *p_points, int pts_count);
	void      (*rlib_draw_rect)(hlayer_t h_layer, int x, int y, int width, int height, int fillmode);
	void      (*rlib_draw_polygon)(hlayer_t h_layer, const point_t *p_points, int pts_count, int fillmode);
	void      (*rlib_draw_circle)(hlayer_t h_layer, int x, int y, int radius, color_t color);
} rlib_dt_t;

const rlib_dt_t *get_raster_api(int version);