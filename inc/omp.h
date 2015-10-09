#ifndef omp_h
#define omp_h

#include <stdarg.h>
#include <omp.h>

#include "julia.h"

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
);

#endif
