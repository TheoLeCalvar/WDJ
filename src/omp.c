#include "legacy.h"
#include "omp.h"
#include <stdio.h>

void omp(char* pixels, int w, int h, mpfr_t minR, mpfr_t minI, mpfr_t maxR, mpfr_t maxI, mpfr_t cR, mpfr_t cI, int iter) {
    mpfr_t rangR, rangI;
    int count = 0;

    mpfr_inits(rangR, rangI, NULL);

    mpfr_sub(rangR, maxR, minR, MPFR_RNDN);
    mpfr_sub(rangI, maxI, minI, MPFR_RNDN);
    mpfr_div_ui(rangR, rangR, (unsigned long) w, MPFR_RNDN);
    mpfr_div_ui(rangI, rangI, (unsigned long) h, MPFR_RNDN);


    #pragma omp parallel for schedule(dynamic,1)
    for (int i = 0; i < w; i++) {
        mpfr_t bR;

        mpfr_init(bR);
        mpfr_mul_ui(bR, rangR, (unsigned long) i, MPFR_RNDN);
        mpfr_add(bR, bR, minR, MPFR_RNDN);

        // #pragma omp parallel for schedule(static, 192)
        for (int j = 0; j < h; j++) {
            int r;
            mpfr_t bI;

            mpfr_init(bI);

            mpfr_mul_ui(bI, rangI, (unsigned long) j, MPFR_RNDN);
            mpfr_add(bI, bI, minI, MPFR_RNDN);

            // mpfr_printf("%.128RNf + %.128RNfi\n", bR, bI);

            r = iterateOverJulia(bR, bI, cR, cI, iter);

            if (r >= 0) {
                // int2rgb(pixels, (j + (i * w)) * 3, r);
                pixels[(i + (j * w)) * 3] = (char)r;
                pixels[((i + (j * w)) * 3) + 1] = (char)r;
                pixels[((i + (j * w)) * 3) + 2] = (char)r;
            } else {
                pixels[(i + (j * w)) * 3] = 255;
                pixels[((i + (j * w)) * 3) + 1] = 255;
                pixels[((i + (j * w)) * 3) + 2] = 255;
            }

            mpfr_clear(bI);
            mpfr_free_cache();
        }

        mpfr_clear(bR);
        mpfr_free_cache();
        count = count + 1;
        // if (omp_get_thread_num() == 0)
        // printf("Thread %d - Ended line : %d - %d/%d\n", omp_get_thread_num(), i, count, w);
        printf("%d/%d\r", count, w);
        fflush(stdout);
    }

    mpfr_clears(rangR, rangI, NULL);
}
