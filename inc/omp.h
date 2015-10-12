#ifndef omp_h
#define omp_h

#include <omp.h>
#include "julia.h"
#include "task.h"

void omp(
    char* pixels,
    int w,
    int h,
    tasks_t * tasks,
    int taskIdx,
    double cR,
    double cI,
    int iter
);

#endif
