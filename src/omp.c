#include <stdio.h>
#include "legacy.h"
#include "omp.h"


void val2RGB(char * pixel, int val){
	char r = (char) val;
	
	if (val < 256){
		pixel[0] = 255 - r;
		pixel[1] = r;
		pixel[2] = 0;
	}
	else if (val < 512){
		pixel[0] = 0;
		pixel[1] = 255 - r;
		pixel[2] = r;
	}
	else if (val < 640){
		pixel[0] = r;
		pixel[1] = 0;
		pixel[2] = 255 - r;
	}
	else{
		pixel[0] = 128;
		pixel[1] = 0;
		pixel[2] = 128;
	}
}


void val2grey(char * pixel, int val){
   pixel[0] = (char)val;
   pixel[1] = (char)val;
   pixel[2] = (char)val;
}


void omp(
    char* pixels,
    int w,
    int h,
    tasks_t * t,
    int taskIdx,
    double cR,
    double cI,
    int iter
) {
    double rangR, rangI;
    int size = w * h;
    double minR = t->minR[taskIdx];
    double minI = t->minI[taskIdx];

    rangR = t->maxR[taskIdx] - minR;
    rangI = t->maxI[taskIdx] - minI;

    rangR /= w;
    rangI /= h;

    #pragma omp parallel for schedule(dynamic, 64)
    for (int k = 0; k < size; ++k) {
        int r;
        int     i = k % w;
        int     j = k / w;
        double  bR = minR + rangR * i;
        double  bI = minI + rangI * j;

        r = iterateOverJulia(bR, bI, cR, cI, iter);

        if (r >= 0) {
			val2RGB(pixels + (k * 3), r);
        } else {
            pixels[k * 3]     = 128;
            pixels[k * 3 + 1] = 0;
            pixels[k * 3 + 2] = 128;
        }
    }
}
