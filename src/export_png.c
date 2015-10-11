#include "export_png.h"
#include <stdio.h>
#include <stdlib.h>


void pixels2PNG(const char *pixels, int w, int h, const char *path){
	FILE * f = NULL;
	png_structp png_ptr  = NULL;
	png_infop   info_ptr = NULL;

	if ((f = fopen(path, "wb")) == NULL){
		fprintf(stderr, "! Error : file %s cannot be opened\n", path);
		exit(EXIT_FAILURE);
	}

	// Structure initialization
	if ((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL){
		fprintf(stderr, "! Failed to allocate png structure\n");
		exit(EXIT_FAILURE);
	}

	// Info struct initialization
	if ((info_ptr = png_create_info_struct(png_ptr)) == NULL){
		fprintf(stderr, "! Failed to initialize info structure\n");
		exit(EXIT_FAILURE);
	}

	// Exception handling
	if (setjmp(png_jmpbuf(png_ptr))){
		fprintf(stderr, "! Error during PNG export\n");
		exit(EXIT_FAILURE);
	}

	png_init_io(png_ptr, f);

	// Header
	png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// Actual PNG writing
	for (int y=0; y<h; ++y){
		png_write_row(png_ptr, (unsigned char *) pixels + (y * w * 3));
	}

	png_write_end(png_ptr, NULL);

	fclose(f);

	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_write_struct(&png_ptr, NULL);
}
