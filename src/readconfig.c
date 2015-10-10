#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpfr.h>

#include "readconfig.h"

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
) {
    FILE *f = fopen(fn, "r");

    if (f == NULL) {
        fprintf(stderr, "file does not exist %s\n", fn);
        return -1;
    }

    const size_t line_size = 128;
    char *line = malloc(line_size);
    char *buffer = malloc(line_size);

    while (fgets(line, line_size, f) != NULL)  {
        if (sscanf(line, "W %d", w)) {
            fprintf(stdout, "Width : %d\n", *w);
            continue;
        }

        if (sscanf(line, "H %d", h)) {
            fprintf(stdout, "Height : %d\n", *h);
            continue;
        }

        if (sscanf(line, "i %d", i)) {
            fprintf(stdout, "Iterations : %d\n", *i);
            continue;
        }

        if (sscanf(line, "p %d", p)) {
            fprintf(stdout, "Precision : %d\n", *p);
            mpfr_set_default_prec(*p);
            mpfr_set_prec(minR, *p);
            mpfr_set_prec(maxR, *p);
            mpfr_set_prec(minI, *p);
            mpfr_set_prec(maxI, *p);
            continue;
        }

        // TODO : use algo_index()
        if (sscanf(line, "a %d", a)) {
            fprintf(stdout, "Algorithm : %d\n", *a);
            continue;
        }

        if (sscanf(line, "o %s", o)) {
            fprintf(stdout, "Output : %s\n", o);
            continue;
        }

        if (sscanf(line, "minR %s", buffer)) {
            fprintf(stdout, "minR : %s\n", buffer);
            mpfr_set_str(minR, buffer, 0, MPFR_RNDN);
            continue;
        }

        if (sscanf(line, "maxR %s", buffer)) {
            fprintf(stdout, "maxR : %s\n", buffer);
            mpfr_set_str(maxR, buffer, 0, MPFR_RNDN);
            continue;
        }

        if (sscanf(line, "minI %s", buffer)) {
            fprintf(stdout, "minI : %s\n", buffer);
            mpfr_set_str(minI, buffer, 0, MPFR_RNDN);
            continue;
        }

        if (sscanf(line, "maxI %s", buffer)) {
            fprintf(stdout, "maxI : %s\n", buffer);
            mpfr_set_str(maxI, buffer, 0, MPFR_RNDN);
            continue;
        }

        if (sscanf(line, "cR %s", buffer)) {
            fprintf(stdout, "cR : %s\n", buffer);
            mpfr_set_str(cR, buffer, 0, MPFR_RNDN);
            continue;
        }

        if (sscanf(line, "cI %s", buffer)) {
            fprintf(stdout, "cI : %s\n", buffer);
            mpfr_set_str(cI, buffer, 0, MPFR_RNDN);
            continue;
        }
    }

    free(line);
    free(buffer);

    return  0;
}
