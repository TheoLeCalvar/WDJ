#include "julia.h"


int iterateOverJulia(mpfr_t r, mpfr_t i, mpfr_t cr, mpfr_t ci, int iterations) {
// float x = (z.x * z.x - z.y * z.y) + c.x;
// float y = (z.x * z.y + z.x * z.y) + c.y;
//
// if ((x * x + y * y) > 10.0) break;
// z.x = x;
// z.y = y;
    mpfr_t r_, i_, r2, i2, ri, ir2;

    mpfr_inits(r2, i2, ri, ir2, NULL);
    mpfr_init_set(r_, r, MPFR_RNDN);
    mpfr_init_set(i_, i, MPFR_RNDN);

    for (int j = 0; j < iterations; ++j) {
        mpfr_mul(r2, r_, r_, MPFR_RNDN);
        mpfr_mul(i2, i_, i_, MPFR_RNDN);
        mpfr_mul(ri, r_, i_, MPFR_RNDN);

        mpfr_add(ir2, r2, i2, MPFR_RNDN);

        if (mpfr_cmp_ui(ir2,(unsigned long) 10) > 0) {
            return j;
        }

        mpfr_mul_ui(ri, ri, (unsigned long)2, MPFR_RNDN);
        mpfr_add(i_, ri, ci, MPFR_RNDN);

        mpfr_sub(r2, r2, i2, MPFR_RNDN);
        mpfr_add(r_, r2, cr, MPFR_RNDN);
    }

    mpfr_clears(r_, i_, r2, i2, ri, ir2, NULL);

    return -1;
}
