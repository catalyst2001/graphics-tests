#include "rasterlib.h"
#include "dheap.h"

malloc_func mallocfn = malloc;
calloc_func callocfn = calloc;

typedef struct LAYER_s {
	hbitmap_t h_bitmap;
	//DHEAP_DECLARE(hbitmap_t, bitmap_stack);
	//DHEAP_DECLARE(int, pen_colors_stack);
	//DHEAP_DECLARE(int, brush_colors_stack);
} LAYER_t;
RLIBNO(sizeof(LAYER_t));

typedef struct BITMAP_s {
	bitmap_info_t bmpinf;
	int bytes_per_pixel;
	size_t total_bytes;
	uchar_t *p_data;
} BITMAP_t;

int attributes = 0;
STACK_FIXED_DECLARE(int, attribs_stack, 256);
DHEAP_DECLARE(hlayer_t, layers);

#define RLIB_ARRSIZE(x) (sizeof(x) / sizeof(x[0]))

// ------------------- IMAGE LOADERS ----------------------
rlib_image_processor_dt_t builtin_processors[] = {
	{ .p_format_name = BMP_IMAGE, .rlib_image_import_routine = 0, .rlib_image_export_routine = 0 },
	{ .p_format_name = TGA_IMAGE, .rlib_image_import_routine = 0, .rlib_image_export_routine = 0 },
	{ .p_format_name = PCX_IMAGE, .rlib_image_import_routine = 0, .rlib_image_export_routine = 0 },
	{ .p_format_name = DDS_IMAGE, .rlib_image_import_routine = 0, .rlib_image_export_routine = 0 }
};
RLIBNO(sizeof(builtin_processors) / sizeof(builtin_processors[0]));

int current_img_proc = RLIB_ARRSIZE(builtin_processors);
rlib_image_processor_dt_t *p_imgprocessors[48] = {
	&builtin_processors[0], //BMP_IMAGE
	&builtin_processors[1], //TGA_IMAGE
	&builtin_processors[2], //PCX_IMAGE
	&builtin_processors[3], //DDS_IMAGE
	//...
};

// ------------------- FILTERS ----------------------
rlib_image_filter_dt_t builtin_filters[] = {
	{ .p_filter_name = FILTER_INVERSE, .rlib_apply_filter_routine = 0 },
	{ .p_filter_name = FILTER_SEPIA, .rlib_apply_filter_routine = 0 }
};
RLIBNO(sizeof(builtin_filters) / sizeof(builtin_filters[0]));

int current_img_filter = RLIB_ARRSIZE(builtin_filters);
rlib_image_filter_dt_t *p_imgfilters[48] = {
	&builtin_filters[0],
	&builtin_filters[1],
	//...
};

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
void      rlib_bitmap_get_info(bitmap_info_t *p_dstinfo, hbitmap_t hbitmap);

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
void      rlib_apply_filter_ex(hbitmap_t h_bitmap, const char *p_filtername, int flags, const rect_t *p_rect, rlibobj_t info);
void      rlib_bitmap_inverse_filter(hbitmap_t h_bitmap, const rect_t *p_rect);

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
		.rlib_bitmap_get_info = rlib_bitmap_get_info,

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
		.rlib_draw_circle = rlib_draw_circle,

		.rlib_apply_filter_ex = rlib_apply_filter_ex,
		.rlib_bitmap_inverse_filter = rlib_bitmap_inverse_filter,
	};
	return &dt;
}

bool rlib_init(malloc_func p_malloc, calloc_func p_calloc)
{
	DHEAP_INIT(hlayer_t, &layers, 1, 1);
}

void rlib_shutdown()
{
	DHEAP_FREE(&layers);
}

bool rlib_register_image_processor(rlib_image_processor_dt_t *p_imgprocdt, const char *p_formatname)
{
	for (int i = 0; i < current_img_proc; i++)
		if (!strcmp(p_imgprocessors[i]->p_format_name, p_formatname))
			return false;

	p_imgprocessors[current_img_proc++] = p_imgprocdt;
	return true;
}

bool rlib_unregister_image_processor(const char *p_formatname)
{
	for (int i = 0; i < current_img_proc; i++) {
		if (!strcmp(p_imgprocessors[i]->p_format_name, p_formatname)) {
			if (i == (current_img_proc - 1)) {
				current_img_proc--;
				return true;
			}
			p_imgprocessors[i] = p_imgprocessors[--current_img_proc];
			return true;
		}
	}
	return false;
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
	for (int i = 0; i < current_img_proc; i++) {
		if (!strcmp(p_imgprocessors[i]->p_format_name, p_format)) {
			return p_imgprocessors[i]->rlib_image_export_routine(h_bitmap, p_filename);
		}
	}
	return false;
}

hbitmap_t rlib_load_image(const char *p_image)
{
	FILE *fp = fopen(p_image, "rb");
	if (!fp)
		return NULL;

	hbitmap_t h_bitmap;
	for (int i = 0; i < current_img_proc; i++) {
		rewind(fp);
		if ((h_bitmap = p_imgprocessors[i]->rlib_image_import_routine(fp, p_image))) {
			fclose(fp);
			return h_bitmap;
		}
	}
	fclose(fp);
	return NULL;
}

hbitmap_t rlib_create_bitmap(int width, int height, int bpp, int format)
{
	int absbpp = abs(bpp);
	if (absbpp % 2)
		return NULL;

	BITMAP_t *p_bitmap = (BITMAP_t *)calloc(1, sizeof(BITMAP_t));
	bitmap_info_t *p_bitmapinfo = &p_bitmap->bmpinf;
	p_bitmapinfo->bits_per_pixel = absbpp;
	p_bitmapinfo->pixels_format = format;
	p_bitmapinfo->width = abs(width);
	p_bitmapinfo->height = abs(height);
	p_bitmap->bytes_per_pixel = absbpp / 8;
	p_bitmap->total_bytes = (size_t)(p_bitmapinfo->width * p_bitmapinfo->height * p_bitmap->bytes_per_pixel);
	if (!(p_bitmap->p_data = (uchar_t *)calloc(p_bitmap->total_bytes, 1)))
		return NULL;

	return (hbitmap_t)p_bitmap;
}

hbitmap_t rlib_copy_bitmap(hbitmap_t h_bitmap)
{
	bitmap_info_t bmpinf;
	hbitmap_t h_bitmap_copy;
	uchar_t *p_src_pixels, *p_dst_pixels;
	rlib_bitmap_get_info(&bmpinf, h_bitmap);
	if (!(h_bitmap_copy = rlib_create_bitmap(bmpinf.width, bmpinf.height, bmpinf.bits_per_pixel, bmpinf.pixels_format)))
		return NULL;

	if (!(p_src_pixels = rlib_get_bitmap_pixels(h_bitmap)))
		return NULL;

	size_t total_bytes = (size_t)(bmpinf.width * bmpinf.height * (bmpinf.bits_per_pixel / 8));
	p_dst_pixels = rlib_get_bitmap_pixels(h_bitmap_copy);
	memcpy(p_dst_pixels, p_src_pixels, total_bytes);
	return h_bitmap_copy;
}

void rlib_delete_bitmap(hbitmap_t hbitmap)
{
	if (!hbitmap)
		return;

	BITMAP_t *p_bitmap = (BITMAP_t *)hbitmap;
	if (p_bitmap->p_data)
		free(p_bitmap->p_data);

	free(p_bitmap);
}

uchar_t *rlib_get_bitmap_pixels(hbitmap_t hbitmap)
{
	return ((BITMAP_t *)hbitmap)->p_data;
}

void rlib_bitmap_fill_pixel(hbitmap_t hbitmap, int x, int y, color_t color)
{
	BITMAP_t *p_bitmap = (BITMAP_t *)hbitmap;
	int chx = RLIB_CLAMP(x, 0, p_bitmap->bmpinf.width);
	int chy = RLIB_CLAMP(x, 0, p_bitmap->bmpinf.height);

	uchar_t *p_pixel = &p_bitmap->p_data[(chy * p_bitmap->bmpinf.width + chx) * p_bitmap->bytes_per_pixel];
	for (int i = 0; i < p_bitmap->bytes_per_pixel; i++)
		p_pixel[i] = color.bytes[i];
}

void rlib_bitmap_get_info(bitmap_info_t *p_dstinfo, hbitmap_t hbitmap)
{
	if (!hbitmap)
		return;

	BITMAP_t *p_bitmap = (BITMAP_t *)hbitmap;
	*p_dstinfo = p_bitmap->bmpinf;
}

hlayer_t rlib_create_layer(hlayer_t h_parent_layer, int width, int height)
{
	LAYER_t *p_layer = (LAYER_t *)calloc(1, sizeof(LAYER_t));
	if (!p_layer)
		return NULL;

	//DHEAP_INIT(hbitmap_t, &p_layer->bitmap_stack, 1, 1);
	return (hlayer_t)p_layer;
}

void rlib_delete_layer(hlayer_t h_layer)
{
	if (!h_layer)
		return;

	free(h_layer);
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
	switch (rstrop) {
	case SRCCPY: {

		break;
	}


	}
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

void rlib_apply_filter_ex(hbitmap_t h_bitmap, const char *p_filtername, int flags, const rect_t *p_rect, rlibobj_t info)
{
}

void rlib_bitmap_inverse_filter(hbitmap_t h_bitmap, const rect_t *p_rect)
{

}
