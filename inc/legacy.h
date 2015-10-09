#ifndef legacy_h
#define legacy_h

#include <stdarg.h>
#include <mpfr.h>

#include "julia.h"

void legacy(
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
