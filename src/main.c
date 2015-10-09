#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
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
    double,
    double,
    double,
    double,
    double,
    double,
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
    double minR = -2.5;
    double maxR = 2.5;
    double minI = -2;
    double maxI = 2;
    double cR = -0.8;
    double cI = 0.156;
    char verbose = 0;
    char * outputPath = NULL;
    int opt;
    int algo = 0;
    char * pixels = NULL;

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
                printf("Invalid option with doubles. Check them.\n");
                break;
            }

            case 'R':
                sscanf(optarg, "%lf", &maxR);
                break;

            case 'r':
                sscanf(optarg, "%lf", &minR);
                break;

            case 'I':
                sscanf(optarg, "%lf", &maxI);
                break;

            case 'i':
                sscanf(optarg, "%lf", &minI);
                break;

            case 'C':
                sscanf(optarg, "%lf", &cI);
                break;

            case 'c':
                sscanf(optarg, "%lf", &cR);
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
