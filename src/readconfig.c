#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readconfig.h"
#include "choosealgo.h"

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
) {
    FILE *f = fopen(fn, "r");

    if (f == NULL) {
        fprintf(stderr, "Config file does not exist (%s)\n", fn);
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

        if (sscanf(line, "a %s", buffer)) {
            (*a) = algo_index(buffer);
            if ((*a) < 0) {
                fprintf(stderr, "Invalid algorithm. Choose one :\n");
                algo_help();
                return -1;
            }
            fprintf(stdout, "Algorithm : %s\n", buffer);
            continue;
        }

        if (sscanf(line, "minR %lf", minR)) {
            fprintf(stdout, "minR : %lf\n", *minR);
            continue;
        }

        if (sscanf(line, "maxR %lf", maxR)) {
            fprintf(stdout, "maxR : %lf\n", *maxR);
            continue;
        }

        if (sscanf(line, "minI %lf", minI)) {
            fprintf(stdout, "minI : %lf\n", *minI);
            continue;
        }

        if (sscanf(line, "maxI %lf", maxI)) {
            fprintf(stdout, "maxI : %lf\n", *maxI);
            continue;
        }

        if (sscanf(line, "cR %lf", cR)) {
            fprintf(stdout, "cR : %lf\n", *cR);
            continue;
        }

        if (sscanf(line, "cI %lf", cI)) {
            fprintf(stdout, "cI : %lf\n", *cI);
            continue;
        }
    }

    free(line);
    free(buffer);

    return  0;
}
