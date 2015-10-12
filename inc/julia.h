#ifndef julia_h
#define julia_h

#include <stdarg.h>
#include <mpfr.h>

int iterateOverJulia(
    double r,
    double i,
    double cr,
    double ci,
    int iterations
);

#endif
