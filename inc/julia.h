#ifndef julia_h
#define julia_h

#include <stdarg.h>
#include <mpfr.h>

int iterateOverJulia(mpfr_t r, mpfr_t i, mpfr_t cr, mpfr_t ci, int iterations);

#endif
