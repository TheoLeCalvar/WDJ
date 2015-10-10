#ifndef task_h
#define task_h

typedef struct tasks_t {
    double minR[2047];
    double maxR[2047];
    double minI[2047];
    double maxI[2047];
    long finalTask;
    long nextTask;
    //pading on 64 bytes
    long foo;
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
