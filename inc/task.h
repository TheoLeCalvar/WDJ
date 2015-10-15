#ifndef task_h
#define task_h

#include <pthread.h>

#define MINR(tab, i) tab[(i * 4)]
#define MAXR(tab, i) tab[(i * 4) + 1]
#define MINI(tab, i) tab[(i * 4) + 2]
#define MAXI(tab, i) tab[(i * 4) + 3]

#define ASK4TASKS 42
#define TASKS_ANNOUNCEMENT 43
#define TASKS_SENDING 44

typedef struct tasks_t {
    double *bound;
    long finalTask;
    long offset;
    long bar;
	long curTask;
} tasks_t;

pthread_mutex_t tasksMutex;

void getTasks( tasks_t * t,
                double minR, double maxR,
                double minI, double maxI,
                int width, int height,
                int blockWidth, int blockHeight,
                int usempi);

char askForTasks(tasks_t * t);
void * giveTasks(tasks_t * t);

#endif
