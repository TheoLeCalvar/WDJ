#include <mpi.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <png.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>
#include <quadmath.h>


#include "legacy.h"
#include "omp.h"
#include "julia.h"
#include "export_png.h"

#include "task.h"
#include "log.h"
#include "choosealgo.h"
#include "readconfig.h"

struct option options[] = {
    {"algo",        0, NULL, 'a'},
    {"blockWidth",  0, NULL, 'b'},
    {"blockHeight", 0, NULL, 'B'},
    {"cstR",        0, NULL, 'c'},
    {"cstI",        0, NULL, 'C'},
    {"help",        0, NULL, 'h'},
    {"height",      0, NULL, 'H'},
    {"minI",        0, NULL, 'i'},
    {"maxI",        0, NULL, 'I'},
    {"use-mpi",     0, NULL, 'm'},
    {"iteration",   0, NULL, 'n'},
    {"color",       0, NULL, 'o'},
    {"minR",        0, NULL, 'r'},
    {"maxR",        0, NULL, 'R'},
    {"width",       0, NULL, 'W'},
    {0,             0,    0,   0}
};

void usage() {
    printf("Ici on mettra l'usage quand on aura le temps =).\n");
    algo_help();
}

int main(int argc, char * argv[]) {
    int opt;
    int width       = 1024;
    int height      = 1024;
    int blockWidth  = 512;
    int blockHeight = 512;
    int iterations  = 1000;
    int algo        = 0;
    int zoom        = 0;
    int usempi      = 0;
    int colorized   = 0;
    double minR     = -2.5;
    double maxR     = 2.5;
    double minI     = -2;
    double maxI     = 2;
    double cR       = -0.8;
    double cI       = 0.156;
    char * pixels   = NULL;
    char hostname[256];
    tasks_t tasks;
    pthread_t thread;
	pthread_t jamy;

    while (
        (opt = getopt_long(
            argc,
            argv,
            "r:R:i:I:c:C:n:W:H:b:B:a:moh",
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

            case 'a':
                algo = algo_index(optarg);
                if (algo < 0) {
                    printf("wrong algorithm. Choose one :\n");
                    algo_help();
                    return -1;
                }
                break;

            case 'm':
                usempi = 1;
                break;

            case 'o':
                colorized = 1;
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

    if (usempi) {
        MPI_Init(&argc, &argv);
    }

    getTasks(   &tasks,
                minR, maxR,
                minI, maxI,
                width, height,
                blockWidth, blockHeight,
                usempi
            );

    if (usempi)
        gethostname(hostname, 256);

    if ((pixels = malloc(3 * blockWidth * blockHeight * sizeof(char))) == NULL){
		log_err("Failed to allocate pixels matrix");
		exit(EXIT_FAILURE);
	}
    zoom = log2(width / blockWidth);

    struct pixelsWriteArgs pargs;
    pargs.w = blockWidth;
    pargs.h = blockHeight;

    pthread_mutex_init(&pixelsBufferMutex, NULL);
    pthread_create(&thread, NULL, writePixelsBuffer, &pargs);

	if (usempi){
		// Tasks giving
		pthread_mutex_init(&tasksMutex, NULL);
		pthread_create(&jamy, NULL, (void * (*)(void *))giveTasks, (void *)&tasks);
	}

	do{
		for (tasks.curTask = tasks.offset; tasks.curTask <= tasks.finalTask;) {
			char fileName[256];
			
			int blockX = (tasks.curTask) % (width / blockWidth);
			int blockY = (tasks.curTask) / (width / blockWidth);

			algos[algo].func(
				pixels,
				blockWidth,
				blockHeight,
				&tasks,
				tasks.curTask,
				cR,
				cI,
				iterations,
				colorized
			);


			log_info("Task %d (%d,%d) done with success on %s.", tasks.curTask, blockY, blockX, hostname);
			snprintf(fileName, 256, "res/images/%d-%d-%d.png", zoom, blockY, blockX);

			pushPixelsBuffer(pixels, blockWidth, blockHeight, fileName);

			pthread_mutex_lock(&tasksMutex);
			++tasks.curTask;
			pthread_mutex_unlock(&tasksMutex);
		}

	}while(askForTasks(&tasks));

    pixelsWritterShouldStop = 1;
	pthread_cancel(jamy);
    pthread_join(thread, NULL);
	pthread_mutex_destroy(&tasksMutex);
    pthread_mutex_destroy(&pixelsBufferMutex);

	free(tasks.bound);

    if (usempi)
        MPI_Finalize();

    free(pixels);

    return 0;
}
