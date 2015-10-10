#ifndef __read_config_h
#define __read_config_h
#include <mpfr.h>

int readconfigfile(
    int *w,
    int *h,
    int *i,
    int *p,
    mpfr_t minR,
    mpfr_t maxR,
    mpfr_t minI,
    mpfr_t maxI,
    mpfr_t cR,
    mpfr_t cI,
    char *o,
    int *a,
    char *fn
);

#endif
