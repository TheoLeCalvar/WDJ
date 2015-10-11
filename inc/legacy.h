#ifndef legacy_h
#define legacy_h

#include <mpfr.h>
#include "julia.h"

void legacy(char* pixels, int w, int h, mpfr_t minR, mpfr_t minI, mpfr_t maxR, mpfr_t maxI, mpfr_t cR, mpfr_t cI, int iter);

#endif
