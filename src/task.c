#include "task.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

void getTasks( tasks_t * t,
                double minR, double maxR,
                double minI, double maxI,
                int width, int height, int blockWidth, int blockHeight) {
    int nbNodes;
    int rank;
    int nbBlocks, blocksPerNode;
    int firstBlock, lastBlock;
    int blocksPerLine, blocksPerColumn;
    double rangR, rangI;

    rangR = maxR - minR;
    rangI = maxI - minI;

    rangR /= width;
    rangI /= height;


    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nbNodes);

    if ((width % blockWidth != 0) || (height % blockHeight != 0)) {
        log_err( "Width or height not compatible with block size.\n"
                "Please choose an image size multiple of the block size.");
        goto error;
    }
    if ((blockWidth == 0) || (blockHeight == 0)) {
        log_err("Block size cannot be 0, check your configuration.");
        goto error;
    }

    blocksPerLine = width / blockWidth;
    blocksPerColumn = height / blockHeight;
    nbBlocks = blocksPerLine * blocksPerColumn;
    blocksPerNode = nbBlocks / nbNodes;
    firstBlock = blocksPerNode * rank;
    lastBlock = blocksPerNode * (rank + 1) - 1;

    /* If the number of block cannot be divided by the number of node
     * we affect the remaining blocks to the last node.
     */
    if ((nbBlocks % nbNodes != 0) && (rank == (nbNodes - 1))) {
        lastBlock = nbBlocks - 1;
    }

    log_info("Dividing the (%dx%d) image in %d blocks of (%dx%d).", width, height, nbBlocks, blockWidth, blockHeight);
    log_info("Node %d, %d total nodes.", rank, nbNodes);
    log_info("Taking care of blocks %d to %d (%d blocks assigned).", firstBlock, lastBlock, lastBlock - firstBlock + 1);

    t->finalTask = lastBlock;
    t->offset = firstBlock;

    for (int i = firstBlock, j = 0; i <= lastBlock; ++i, ++j) {
        int blockX = i % blocksPerLine;
        int blockY = i / blocksPerLine;

        log_info("Block %d (%d,%d) for node %d.", i, blockX, blockY, rank);

        t->minR[j] = minR + rangR * blockX * blockWidth;
        t->maxR[j] = minR + rangR * (blockX + 1) * blockWidth;


        t->minI[j] = minI + rangI * blockY * blockHeight;
        t->maxI[j] = minI + rangI * (blockY + 1) * blockHeight;

        log_info("Task for block %d : minR %lf, maxR %lf, minI %lf, maxI %lf.",
                    i, t->minR[j], t->maxR[j], t->minI[j], t->maxI[j]);
    }

    return;

error:

    MPI_Finalize();

    exit(0);
}

void askForTasks(tasks_t * t);
void giveTasks(tasks_t * t);

void sendResult(int taskIdx, char * pixels) {

}

void receiveResult() {

}
