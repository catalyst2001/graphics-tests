#include "rasterlib.h"

malloc_func mallocfn = malloc;
calloc_func callocfn = calloc;

bool bitmap_create(bitmap_t *p_dstbitmap, int format, int bpp, int width, int height)
{
	if (!p_dstbitmap)
		return false;

	p_dstbitmap->pixels_format = format;
	p_dstbitmap->bits_per_pixel = abs(bpp);
	if (p_dstbitmap->bits_per_pixel % 2)
		return false;

	p_dstbitmap->bytes_per_pixel = p_dstbitmap->bits_per_pixel / 8;
	p_dstbitmap->width = abs(width);
	p_dstbitmap->height = abs(height);
	p_dstbitmap->total_bytes = (size_t)(p_dstbitmap->width * p_dstbitmap->height * p_dstbitmap->bytes_per_pixel);
	p_dstbitmap->p_data = (uchar_t *)callocfn(p_dstbitmap->total_bytes, 1);
	if (!p_dstbitmap->p_data)
		return false;
	
	return true;
}

bool bitmap_flip(const bitmap_t *p_srcbitmap, int flipop)
{
	return false;
}

bool bitmap_rot(const bitmap_t *p_srcbitmap, int rotop)
{
	return false;
}

void bitmap_free(const bitmap_t *p_srcbitmap)
{
}

bool bitmap_bitblt(bitmap_t *p_dstbmp, int xdst, int ydst, const bitmap_t *p_srcbmp, int xsrc, int ysrc, int rastrop)
{
	return false;
}
