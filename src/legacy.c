#include <stdio.h>
#include "legacy.h"
#include "color.h"

void legacy(
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
                if (colorized) {
                    val2RGB(pixels + (i + (j * w) * 3), r);
                } else {
                    val2Grey(pixels + (i + (j * w) * 3), r);
                }
            } else {
                if (colorized) {
                    val2RGB(pixels + (i + (j * w) * 3), 640);
                } else {
                    val2Grey(pixels + (i + (j * w) * 3), 640);
                }
            }
        }

        fprintf(stdout, "leg : %d/%d\r", i, w);
        fflush(stdout);
    }
}
