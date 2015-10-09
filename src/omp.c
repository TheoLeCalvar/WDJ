#include <stdio.h>
#include "legacy.h"
#include "omp.h"

void omp(
    char* pixels,
    int w,
    int h,
    mpfr_t minR,
    mpfr_t minI,
    mpfr_t maxR,
    mpfr_t maxI,
    mpfr_t cR,
    mpfr_t cI,
    int iter
) {
    mpfr_t rangR, rangI;
    int i,j;

    mpfr_inits(rangR, rangI, NULL);

    mpfr_sub(rangR, maxR, minR, MPFR_RNDN);
    mpfr_sub(rangI, maxI, minI, MPFR_RNDN);
    mpfr_div_ui(rangR, rangR, (unsigned long) w, MPFR_RNDN);
    mpfr_div_ui(rangI, rangI, (unsigned long) h, MPFR_RNDN);

    for (i = 0; i < w; i++) {
        mpfr_t bR;

        mpfr_init(bR);
        mpfr_mul_ui(bR, rangR, (unsigned long) i, MPFR_RNDN);
        mpfr_add(bR, bR, minR, MPFR_RNDN);

        #pragma omp parallel for shared(bR) private(j) schedule(dynamic, 16)
        for (j = 0; j < h; j++) {
            int r;
            mpfr_t bI;

            mpfr_init(bI);

            mpfr_mul_ui(bI, rangI, (unsigned long) j, MPFR_RNDN);
            mpfr_add(bI, bI, minI, MPFR_RNDN);

            // mpfr_printf("%.128RNf + %.128RNfi\n", bR, bI);

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

            mpfr_clear(bI);
        }
        #pragma omp nowait

        mpfr_clear(bR);
    }

    mpfr_clears(rangR, rangI, NULL);
}
