#include <stdio.h>
#include "legacy.h"

void legacy(
    char* pixels,
    int w,
    int h,
    double minR,
    double minI,
    double maxR,
    double maxI,
    double cR,
    double cI,
    int iter
) {
    double rangR, rangI, bR, bI;
    int i, j, r;

    rangR = maxR - minR;
    rangI = maxI - minI;

    rangR = rangR / w;
    rangI = rangI / h;

    for (i = 0; i < w; i++) {
        bR = rangR * i;
        bR += minR;

        for (j = 0; j < h; j++) {
            bI = rangI * j;
            bI += minI;

            r = iterateOverJulia(bR, bI, cR, cI, iter);

            if (r >= 0) {
                pixels[(i + (j * w)) * 3] = (char)r;
                pixels[((i + (j * w)) * 3) + 1] = (char)r;
                pixels[((i + (j * w)) * 3) + 2] = (char)r;
            } else {
                pixels[(i + (j * w)) * 3] = 0;
                pixels[((i + (j * w)) * 3) + 1] = 0;
                pixels[((i + (j * w)) * 3) + 2] = 0;
            }
        }
    }
}
