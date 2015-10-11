#include <mpi.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <png.h>
#include <unistd.h>
#include <sys/stat.h>

#include "legacy.h"
#include "omp.h"
#include "julia.h"
#include "export_png.h"

#include "task.h"

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
    {"blockWidth",  0, NULL, 'b'},
    {"blockHeight", 0, NULL, 'B'},
    {"algo",        0, NULL, 'a'},
    {"verbose",     0, NULL, 'v'},
    {"help",        0, NULL, 'h'},
    {0,             0, 0,     0}
};

typedef void (*algo_f)(
    char *,
    int,
    int,
    tasks_t *,
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
    char * outputPath = NULL;
    int opt;
    int algo = 0;
    char * pixels = NULL;
    char hostname[256];
    tasks_t tasks;

    while (
        (opt = getopt_long(
            argc,
            argv,
            "r:R:i:I:c:C:n:o:W:H:b:B:p:a:vh",
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

            case 'b':
                blockHeight = atoi(optarg);
                break;

            case 'B':
                blockWidth = atoi(optarg);
                break;

            case 'v':
                verbose = 1;
                break;

            case 'o':
                outputPath = optarg;
                printf("This option has no effect. Output will be in res/\n");
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

    MPI_Init( &argc, &argv);
    getTasks(   &tasks,
                minR, maxR,
                minI, maxI,
                width, height,
                blockWidth, blockHeight
            );
    gethostname(hostname, 256);

    pixels = malloc(3 * blockWidth * blockHeight * sizeof(char));

    for (; tasks.nextTask <= tasks.finalTask; ++tasks.nextTask) {
        char fileName[256];
        char dirname1[256];
        char dirname2[256];

        int blockX = tasks.nextTask % (width / blockWidth);
        int blockY = tasks.nextTask / (width / blockWidth);

        algos[algo].func(
            pixels,
            blockWidth,
            blockHeight,
            &tasks,
            cR,
            cI,
            iterations
        );

        printf("Task %ld done with success on %s.\n", tasks.nextTask, hostname);

        // sprintf(dirname1, "res/Map0/LayerA/Zoom0/%d", blockY);
        // sprintf(dirname2, "%s/%d", dirname1, blockX);
        sprintf(fileName, "res/images/3-%d-%d.png", blockX, blockY);

        // mkdir(dirname1, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        // mkdir(dirname2, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        pixels2PNG(pixels, blockWidth, blockHeight, fileName);
    }


    free(pixels);
    MPI_Finalize();

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
