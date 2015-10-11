#include <string.h>
#include <stdio.h>
#include "choosealgo.h"

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
