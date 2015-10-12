#include <mpi.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <png.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#include "legacy.h"
#include "omp.h"
#include "julia.h"
#include "export_png.h"

#include "task.h"
#include "log.h"
#include "choosealgo.h"
#include "readconfig.h"

struct option options[] = {
    {"minR",        0, NULL, 'r'},
    {"maxR",        0, NULL, 'R'},
    {"minI",        0, NULL, 'i'},
    {"maxI",        0, NULL, 'I'},
    {"cstR",        0, NULL, 'c'},
    {"cstI",        0, NULL, 'C'},
    {"iteration",   0, NULL, 'n'},
    {"width",       0, NULL, 'W'},
    {"height",      0, NULL, 'H'},
    {"blockWidth",  0, NULL, 'b'},
    {"blockHeight", 0, NULL, 'B'},
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
    int blockWidth = 16;
    int blockHeight = 16;
    int iterations = 1000;
    double minR = -2.5;
    double maxR = 2.5;
    double minI = -2;
    double maxI = 2;
    double cR = -0.8;
    double cI = 0.156;
    char verbose = 0;
    int opt;
    int algo = 0;
    char * pixels = NULL;
    char hostname[256];
    int zoom = 0;
    tasks_t tasks;

    while (
        (opt = getopt_long(
            argc,
            argv,
            "r:R:i:I:c:C:n:W:H:b:B:a:vh",
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
                      &minR,
                      &maxR,
                      &minI,
                      &maxI,
                      &cR,
                      &cI,
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

            case 'b':
                blockHeight = atoi(optarg);
                break;

            case 'B':
                blockWidth = atoi(optarg);
                break;

            case 'v':
                verbose = 1;
                break;

            case 'a':
                algo = algo_index(optarg);
                if (algo < 0) {
                    printf("wrong algorithm. Choose one :\n");
                    algo_help();
                    return -1;
                }
                break;

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

    MPI_Init( &argc, &argv);
    getTasks(   &tasks,
                minR, maxR,
                minI, maxI,
                width, height,
                blockWidth, blockHeight
            );
    gethostname(hostname, 256);

    pixels = malloc(3 * blockWidth * blockHeight * sizeof(char));
    zoom = log2(width / blockWidth);

    for (int t = 0; t + tasks.offset <= tasks.finalTask; ++t) {
        char fileName[256];

        int blockX = (t + tasks.offset) % (width / blockWidth);
        int blockY = (t + tasks.offset) / (width / blockWidth);

        algos[algo].func(
            pixels,
            blockWidth,
            blockHeight,
            &tasks,
            t,
            cR,
            cI,
            iterations
        );

        log_info("Task %d (%d,%d) done with success on %s.", t, blockY, blockX, hostname);
        sprintf(fileName, "res/images/%d-%d-%d.png", zoom, blockY, blockX);
        pixels2PNG(pixels, blockWidth, blockHeight, fileName);
    }

    free(pixels);
    MPI_Finalize();

    return 0;
}
