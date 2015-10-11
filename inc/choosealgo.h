#ifndef __choosealgo_h
#define __choosealgo_h

#include "legacy.h"
#include "omp.h"

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

static const algo_t algos[] = {
    {"leg", "legacy algorithm",       &legacy},
    {"omp", "openMP algorithm",       &omp},
    {"mpi", "openMP + MPI algorithm", NULL}
};

#define ALGO_SIZE (sizeof(algos) / sizeof(algo_t))

int algo_index(const char* name);
void algo_help(void);

#endif
