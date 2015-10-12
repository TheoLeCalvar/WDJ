#ifndef __read_config_h
#define __read_config_h

#include <mpfr.h>

int readconfigfile(
    int *w,
    int *h,
    int *i,
    double *minR,
    double *maxR,
    double *minI,
    double *maxI,
    double *cR,
    double *cI,
    int *a,
    char *fn
);

#endif
