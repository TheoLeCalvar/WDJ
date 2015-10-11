#ifndef task_h
#define task_h

typedef struct tasks_t {
    double minR[16384];
    double maxR[16384];
    double minI[16384];
    double maxI[16384];
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
