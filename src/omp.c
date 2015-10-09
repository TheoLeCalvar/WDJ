#include <stdio.h>
#include "legacy.h"
#include "omp.h"

void omp(
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
    double rangR, rangI;
    int i, j, count = 0;

    rangR = maxR - minR;
    rangI = maxI - minI;

    rangR /= w;
    rangI /= h;

    for (i = 0; i < h; i++) {
        double bR;

        bR = rangR * i;
        bR += minR;

        #pragma omp parallel for shared(bR) schedule(dynamic, 1)
        for (j = 0; j < w; j++) {
            int r;
            double bI;

            bI = rangI * j;
            bI += minI;

            r = iterateOverJulia(bR, bI, cR, cI, iter);

            if (r >= 0) {
                pixels[(j  + (i * w)) * 3]      = (char)r;
                pixels[((j + (i * w)) * 3) + 1] = (char)r;
                pixels[((j + (i * w)) * 3) + 2] = (char)r;
            } else {
                pixels[(j  + (i * w)) * 3]      = 0;
                pixels[((j + (i * w)) * 3) + 1] = 0;
                pixels[((j + (i * w)) * 3) + 2] = 0;
            }
        }

        count += 1;
        fprintf(stdout, "%d/%d\r", count, w);
        fflush(stdout);
    }
}
