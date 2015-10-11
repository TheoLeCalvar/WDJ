#ifndef omp_h
#define omp_h

#include <mpfr.h>
#include <omp.h>
#include "julia.h"

void omp(char* pixels, int w, int h, mpfr_t minR, mpfr_t minI, mpfr_t maxR, mpfr_t maxI, mpfr_t cR, mpfr_t cI, int iter);

#endif
