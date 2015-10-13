#ifndef task_h
#define task_h

#define MINR(tab, i) tab[(i * 4)]
#define MAXR(tab, i) tab[(i * 4) + 1]
#define MINI(tab, i) tab[(i * 4) + 2]
#define MAXI(tab, i) tab[(i * 4) + 3]

typedef struct tasks_t {
    double *bound;
    long finalTask;
    long offset;
    long bar;
} tasks_t;

void getTasks( tasks_t * t,
                double minR, double maxR,
                double minI, double maxI,
                int width, int height,
                int blockWidth, int blockHeight);

void askForTasks(tasks_t * t);
void giveTasks(tasks_t * t);

void sendResult(int taskIdx, char * pixels);
void receiveResult();

#endif
