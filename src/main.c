#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <mpfr.h>
#include "legacy.h"
#include "omp.h"
#include "julia.h"

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
    {0,             0, 0,     0}
};

typedef void (*algo_f)(
    char *,
    int,
    int,
    mpfr_t,
    mpfr_t,
    mpfr_t,
    mpfr_t,
    mpfr_t,
    mpfr_t,
    int
);

typedef struct algo {
    char    name[32];
    char    desc[256];
    algo_f  func;
} algo_t;

algo_t algos[] = {
    {"leg", "legacy algorithm",       &legacy},
    {"omp", "openMP algorithm",       &omp},
    {"mpi", "openMP + MPI algorithm", NULL}
};

#define ALGO_SIZE       (sizeof(algos) / sizeof(algo_t))

int algo_index(const char* name) {
    for (unsigned int i = 0; i < ALGO_SIZE; i++) {
        if (!strcmp(name, algos[i].name)) {
            return i;
        }
    }

    return -1;
}

void algo_help(void) {
    for (unsigned int i = 0; i < ALGO_SIZE; i++) {
        printf("    - %-16s : %s\n", algos[i].name, algos[i].desc);
    }
}

void usage() {
    printf("Ici on mettra l'usage quand on aura le temps =).\n");
    algo_help();
}

void pixels2BMP(const char* pixels, int w, int h, const char * path);

int main(int argc, char * argv[]) {
    int width = 1024;
    int height = 1024;
    int iterations = 1000;
    mpfr_t minR, maxR, minI, maxI, cR, cI;
    char verbose = 0;
    char * outputPath = NULL;
    int opt;
    int algo = 0;
    char * pixels = NULL;



    mpfr_inits(minR, maxR, minI, maxI, cR, cI, NULL);

    while (
        (opt = getopt_long(
            argc,
            argv,
            "r:R:i:I:c:C:n:o:W:H:p:a:vh",
            options,
            NULL)
        ) >= 0
    ) {
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

            case 'a':
                algo = algo_index(optarg);
                if (algo < 0) {
                    printf("wrong algorithm. Choose one :\n");
                    algo_help();
                    return -1;
                }
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
    pixels2BMP(pixels, width, height, outputPath);

    mpfr_clears(maxR, minR, maxI, minI, cR, cI, NULL);
    mpfr_free_cache();
    free(pixels);

    return 0;
}

void pixels2BMP(const char* pixels, int w, int h, const char * path) {
    FILE *f = fopen(path, "wb");

    if (!f) {
        return;
    }

    int filesize = 54 + 3*w*h;
    //w is your image width, h is image height, both int


    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);

    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    for(int i=0; i<h; i++)
    {
        fwrite(pixels+(w*(h-i-1)*3),3,w,f);
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }
    fclose(f);
}
