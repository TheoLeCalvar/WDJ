#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <mpfr.h>

#include "julia.h"
#include "legacy.h"
#include "omp.h"
#include "choosealgo.h"
#include "export_bmp.h"
#include "export_png.h"
#include "readconfig.h"

struct option options[] = {
    {"minR",        0, NULL, 'r'},
    {"maxR",        0, NULL, 'R'},
    {"minI",        0, NULL, 'i'},
    {"maxI",        0, NULL, 'I'},
    {"cstR",        0, NULL, 'c'},
    {"cstI",        0, NULL, 'C'},
    {"precision",   0, NULL, 'p'},
    {"iteration",   0, NULL, 'n'},
    {"output",      0, NULL, 'o'},
    {"width",       0, NULL, 'W'},
    {"height",      0, NULL, 'H'},
    {"algo",        0, NULL, 'a'},
    {"verbose",     0, NULL, 'v'},
    {"help",        0, NULL, 'h'},
    {0,             0,    0,   0}
};

void usage() {
    printf("Ici on mettra l'usage quand on aura le temps =).\n");
    algo_help();
}

int main(int argc, char * argv[]) {
    int width = 1024;
    int height = 1024;
    int iterations = 1000;
    int precision;
    mpfr_t minR, maxR, minI, maxI, cR, cI;
    char verbose = 0;
    char * outputPath = malloc(128);
    int opt;
    int algo = 0;
    char * pixels = NULL;

    mpfr_inits(minR, maxR, minI, maxI, cR, cI, NULL);

    while (
        (opt = getopt_long(
            argc,
            argv,
            "r:R:i:I:c:C:n:o:W:H:p:a:f:vh",
            options,
            NULL)
        ) >= 0
    ) {
        switch (opt) {
            case 'h':
                usage();
                return 0;

            case 'f':
                if (readconfigfile(
                      &width,
                      &height,
                      &iterations,
                      &precision,
                      minR,
                      maxR,
                      minI,
                      maxI,
                      cR,
                      cI,
                      outputPath,
                      &algo,
                      optarg ) == -1) {
                    exit(EXIT_FAILURE);
                }
                break;

            case 'n':
                iterations = atoi(optarg);
                break;

            case 'W':
                width = atoi(optarg);
                break;

            case 'H':
                height = atoi(optarg);
                break;

            case 'v':
                verbose = 1;
                break;

            case 'o':
                strcpy(outputPath, optarg);
                break;

            case 'a':
                algo = algo_index(optarg);
                if (algo < 0) {
                    printf("wrong algorithm. Choose one :\n");
                    algo_help();
                    return -1;
                }
                break;

            case 'p': {
                precision = atoi(optarg);

                mpfr_set_default_prec(precision);
                mpfr_set_prec(minR, precision);
                mpfr_set_prec(maxR, precision);
                mpfr_set_prec(minI, precision);
                mpfr_set_prec(maxI, precision);

                break;
            }

            case 'R':
                if (mpfr_set_str(maxR, optarg, 0, MPFR_RNDN)) {
                    printf("Invalid number format (%s)\n", optarg);
                    exit(1);
                }
                break;

            case 'r':
                if (mpfr_set_str(minR, optarg, 0, MPFR_RNDN)) {
                    printf("Invalid number format (%s)\n", optarg);
                    exit(1);
                }
                break;

            case 'I':
                if (mpfr_set_str(maxI, optarg, 0, MPFR_RNDN)) {
                    printf("Invalid number format (%s)\n", optarg);
                    exit(1);
                }
                break;

            case 'i':
                if (mpfr_set_str(minI, optarg, 0, MPFR_RNDN)) {
                    printf("Invalid number format (%s)\n", optarg);
                    exit(1);
                }
                break;

            case 'C':
                if (mpfr_set_str(cI, optarg, 0, MPFR_RNDN)) {
                    printf("Invalid number format (%s)\n", optarg);
                    exit(1);
                }
                break;

            case 'c':
                if (mpfr_set_str(cR, optarg, 0, MPFR_RNDN)) {
                    printf("Invalid number format (%s)\n", optarg);
                    exit(1);
                }
                break;

            case '?':
                usage();
                return 1;

            case ':':
                printf("missing parameter\n\n");
                usage();
                return 1;
        }
    }

    pixels = malloc(3 * width * height * sizeof(char));

    if (mpfr_nan_p(cR)) {
        mpfr_set_str(cR, "-0.8", 0, MPFR_RNDN);
    }

    if (mpfr_nan_p(cI)) {
        mpfr_set_str(cI, "0.156", 0, MPFR_RNDN);
    }

    if (mpfr_nan_p(minR)) {
        mpfr_set_str(minR, "-2.5", 0, MPFR_RNDN);
    }

    if (mpfr_nan_p(maxR)) {
        mpfr_set_str(maxR, "2.5", 0, MPFR_RNDN);
    }

    if (mpfr_nan_p(minI)) {
        mpfr_set_str(minI, "-2", 0, MPFR_RNDN);
    }

    if (mpfr_nan_p(maxI)) {
        mpfr_set_str(maxI, "2", 0, MPFR_RNDN);
    }

    algos[algo].func(pixels,
        width,
        height,
        minR,
        minI,
        maxR,
        maxI,
        cR,
        cI,
        iterations
    );

    if (verbose) {
        fprintf(stdout, "Generating image.\n");
    }

    pixels2PNG(pixels, width, height, outputPath);

    mpfr_clears(maxR, minR, maxI, minI, cR, cI, NULL);
    mpfr_free_cache();
    free(pixels);
    free(outputPath);

    return 0;
}
