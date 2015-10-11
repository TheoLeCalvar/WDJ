#include <stdio.h>
#include "legacy.h"
#include "omp.h"

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
            pixels[k * 3]     = (char)r;
            pixels[k * 3 + 1] = (char)r;
            pixels[k * 3 + 2] = (char)r;
        } else {
            pixels[k * 3]     = 0;
            pixels[k * 3 + 1] = 0;
            pixels[k * 3 + 2] = 0;
        }
    }
}
