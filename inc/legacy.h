#ifndef legacy_h
#define legacy_h

#include <stdarg.h>
#include <mpfr.h>

#include "julia.h"
#include "task.h"

void legacy(
    char* pixels,
    int w,
    int h,
    tasks_t * t,
    double cR,
    double cI,
    int iter
);

#endif
