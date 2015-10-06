#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>


#include "julia.h"

struct option options[] = {
    {"minR",        0, NULL, 'r'},
    {"maxR",        0, NULL, 'R'},
    {"minI",        0, NULL, 'i'},
    {"maxI",        0, NULL, 'I'},
    {"precision",   0, NULL, 'p'},
    {"iteration",   0, NULL, 'n'},
    {"output",      0, NULL, 'o'},
    {"width",       0, NULL, 'W'},
    {"height",      0, NULL, 'H'},
    {"verbose",     0, NULL, 'v'},
    {"help",        0, NULL, 'h'},
    {0,             0, 0,     0}
};

void usage() {
    printf("Ici on mettra l'usage quand on aura le temps =).\n");
}

int main(int argc, char * argv[]) {
    int width = 1024;
    int height = 1024;
    int iterations = 10000;
    mpfr_t minR, maxR, minI, maxI;
    char verbose = 0;
    char * outputPath = NULL;
    int opt;


    mpfr_inits(minR, maxR, minI, maxI);

    while ((opt = getopt_long(argc, argv, "r:R:i:I:n:o:W:H:p:vh", options, NULL))
        >= 0) {

            switch (opt) {
                    case 'h':
                        usage();
                        return 0;

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
                        outputPath = optarg;
                        break;

                    case 'p': {
                        int precision = atoi(optarg);

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

                    case '?':
                        usage();
                        return 1;

                    case ':':
                        printf("missing parameter\n\n");
                        usage();
                        return 1;
            }
    }


    return 0;
}