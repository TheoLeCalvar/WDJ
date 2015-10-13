#include <stdio.h>
#include "legacy.h"
#include "omp.h"
#include "color.h"

void omp(
    char* pixels,
    int w,
    int h,
    tasks_t * t,
    int taskIdx,
    double cR,
    double cI,
    int iter,
    int colorized
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

        int i = k % w;
        int j = k / w;
        double bR = minR + rangR * i;
        double bI = minI + rangI * j;

        r = iterateOverJulia(bR, bI, cR, cI, iter);

        if (r >= 0) {
            if (colorized) {
                val2RGB(pixels + (k * 3), r);
            } else {
                val2Grey(pixels + (k * 3), r);
            }
        } else {
            if (colorized) {
                val2RGB(pixels + (k * 3), 640);
            } else {
                val2Grey(pixels + (k * 3), 640);
            }
        }
    }
}
