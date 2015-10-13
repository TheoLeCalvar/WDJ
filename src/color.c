#include "color.h"

void val2RGB(char * pixel, int val){
    char r = (char) val;

    if (val < 256) {
        pixel[0] = 255 - r;
        pixel[1] = r;
        pixel[2] = 0;
    } else if (val < 512) {
        pixel[0] = 0;
        pixel[1] = 255 - r;
        pixel[2] = r;
    } else if (val < 640) {
        pixel[0] = r;
        pixel[1] = 0;
        pixel[2] = 255 - r;
    } else {
        pixel[0] = 128;
        pixel[1] = 0;
        pixel[2] = 128;
    }
}

void val2Grey(char * pixel, int val){
   pixel[0] = (char)val;
   pixel[1] = (char)val;
   pixel[2] = (char)val;
}
