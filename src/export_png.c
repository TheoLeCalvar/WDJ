#include "export_png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>

#include "log.h"

char * 	pixelsBuffer = NULL;
char ** pixelsBufferNames = NULL;
int 	pixelsBufferSize = 0;
int 	pixelsBufferUsed = 0;
char 	pixelsWritterShouldStop = 0;
pthread_mutex_t pixelsBufferMutex;

void pushPixelsBuffer(char * pixels, int w, int h, char * path) {
	int spaceNeeded = w * h * 3;

	pthread_mutex_lock(&pixelsBufferMutex);

	if (pixelsBufferUsed + spaceNeeded >= pixelsBufferSize) {
		char * tmp   = realloc(pixelsBuffer, pixelsBufferSize + 100 * 3 * w * h);
		char ** tmp2 = realloc(pixelsBufferNames, pixelsBufferSize + 100);

		if (!tmp || !tmp2) {
			log_err("Y'a une couille chef !");
			MPI_Abort(MPI_COMM_WORLD, 255);
		}

		pixelsBufferSize += 100;
		pixelsBuffer = tmp;
		pixelsBufferNames = tmp2;
	}

	memcpy(pixelsBuffer + pixelsBufferUsed * h * w * 3, pixels, spaceNeeded * sizeof(char));
	pixelsBufferNames[pixelsBufferUsed] = strndup(path, 256);
	pixelsBufferUsed += 1;

	pthread_mutex_unlock(&pixelsBufferMutex);
}


void * writePixelsBuffer(void * args) {
	struct pixelsWriteArgs * pargs= (struct pixelsWriteArgs*)args;
	int w, h;

	w = pargs->w;
	h = pargs->h;

	while (!pixelsWritterShouldStop || (pixelsBufferUsed > 0)) {
		if (pixelsBufferUsed) {
			pthread_mutex_lock(&pixelsBufferMutex);
			pixels2PNG(	pixelsBuffer + (pixelsBufferUsed - 1) * w * h * 3, w, h,
						pixelsBufferNames[pixelsBufferUsed - 1]
					  );
			free(pixelsBufferNames[pixelsBufferUsed - 1]);
			pixelsBufferUsed -= 1;
			pthread_mutex_unlock(&pixelsBufferMutex);
		}
		else {
			usleep(100);
		}
	}

	return NULL;
}


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

	// NULL is for io_ptr, defined in next function
	png_set_write_fn(png_ptr, NULL, custom_write_data, custom_output_flush);

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
	png_destroy_write_struct(&png_ptr, &info_ptr);
}


// We kept the actual libpng function, to be able to adapt them to MPI
// Only a few modifications were made to be able to compile
void custom_write_data(png_structp png_ptr, png_bytep data, png_size_t length){
	png_FILE_p file = NULL;
	png_size_t check;

	if (png_ptr == NULL)
		return;

	file = (png_FILE_p)png_get_io_ptr(png_ptr);

	check = fwrite(data, 1, length, file);

	if (check != length)
		png_error(png_ptr, "Write Error");
}


void custom_output_flush(png_structp png_ptr){
	png_voidp file = NULL;

	if (png_ptr == NULL)
		return;

	file = png_get_io_ptr(png_ptr);

	fflush(file);
}
