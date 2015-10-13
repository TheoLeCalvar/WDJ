#ifndef __export_png_h
#define __export_png_h

#include <png.h>
#include <pthread.h>


struct pixelsWriteArgs{
    int w;
    int h;
};

char * 	pixelsBuffer;
char ** pixelsBufferNames;
int 	pixelsBufferSize;
int 	pixelsBufferUsed;
char    pixelsWritterShouldStop;
pthread_mutex_t pixelsBufferMutex;

void pushPixelsBuffer(char * pixels, int w, int h, char * path);
void * writePixelsBuffer(void * args);
void pixels2PNG(const char* pixels, int w, int h, const char *path);
void custom_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
void custom_output_flush(png_structp png_ptr);


#endif
