#ifndef __export_png_h
#define __export_png_h

#include <png.h>

void pixels2PNG(const char* pixels, int w, int h, const char * path);
void custom_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
void custom_output_flush(png_structp png_ptr);

#endif
