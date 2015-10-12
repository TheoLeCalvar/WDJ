#include <stdio.h>
#include "legacy.h"

void legacy(
    char* pixels,
    int w,
    int h,
    tasks_t * t,
    int taskIdx,
    double cR,
    double cI,
    int iter
) {
    double rangR, rangI, bR, bI;
    int i, j, r;
    double minR = t->minR[taskIdx];
    double minI = t->minI[taskIdx];

    rangR = t->maxR[taskIdx] - minR;
    rangI = t->maxI[taskIdx] - minI;

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

        fprintf(stdout, "leg : %d/%d\r", i, w);
        fflush(stdout);
    }
}
