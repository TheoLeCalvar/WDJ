#include "task.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

void getTasks( tasks_t * t,
                double minR, double maxR,
                double minI, double maxI,
                int width, int height, int blockWidth, int blockHeight,
                int usempi) {
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

    if (!usempi) {
        nbNodes = 1;
        rank = 0;
    } else {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &nbNodes);
    }

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

	// Min/max array allocation
	if ((t->bound = (double *)malloc((lastBlock - firstBlock + 1) * sizeof(double) * 4)) == NULL){
		log_err("! Allocation failed in a function");
		goto error;
	}

	t->finalTask = lastBlock;
    t->offset = firstBlock;

    for (int i = firstBlock, j = 0; i <= lastBlock; ++i, ++j) {
        int blockX = i % blocksPerLine;
        int blockY = i / blocksPerLine;

        log_info("Block %d (%d,%d) for node %d.", i, blockX, blockY, rank);

        MINR(t->bound, j) = minR + rangR * blockX * blockWidth;
        MAXR(t->bound, j) = minR + rangR * (blockX + 1) * blockWidth;


        MINI(t->bound, j) = minI + rangI * blockY * blockHeight;
        MAXI(t->bound, j) = minI + rangI * (blockY + 1) * blockHeight;

        log_info("Task for block %d : minR %lf, maxR %lf, minI %lf, maxI %lf.",
                    i, MINR(t->bound, j), MAXR(t->bound, j), MINI(t->bound, j), MAXI(t->bound, j));
    }

    return;

error:
    if (usempi)
        MPI_Finalize();

    exit(0);
}

char askForTasks(tasks_t * t){
	int rank, nbNodes;
	long msg[2] = {0}; // Content is useless, we just want to ping the potentiel workgivers
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nbNodes);

	if (nbNodes == 1){
		return(0);
	}
	for (int i = ((rank + 1 == nbNodes)?0:(rank + 1)); i != rank; i = (i + 1) % nbNodes){
//		MPI_Sendrecv_replace(&msg, 1, MPI_INTEGER, i, ASK4TASKS, MPI_ANY_SOURCE, TASKS_ANNOUNCEMENT, MPI_COMM_WORLD, &status);
		MPI_Send(msg, 2, MPI_LONG_INT, i, ASK4TASKS, MPI_COMM_WORLD);
		MPI_Recv(msg, 2, MPI_LONG_INT, MPI_ANY_SOURCE, TASKS_ANNOUNCEMENT, MPI_COMM_WORLD, &status);
		printf("> [%d] Asking %d for tasks\n", rank, i);

		if (msg[0] > 0){
			pthread_mutex_lock(&tasksMutex);

			free(t->bound);
			if ((t->bound = malloc(msg[0] * 4 * sizeof(double))) == NULL){
				log_err("Failed to reallocate tasks array");
				MPI_Abort(MPI_COMM_WORLD, 42);
			}
			MPI_Recv(t->bound, 4 * msg[0], MPI_DOUBLE, MPI_ANY_SOURCE, TASKS_SENDING, MPI_COMM_WORLD, &status);

			t->offset = msg[1];
			t->finalTask = msg[1] + msg[0];

			pthread_mutex_unlock(&tasksMutex);

			return(1);
		}
	}

	// We asked everybody, and noone has work for us
	return(0);
}


void * giveTasks(tasks_t * t){
	long msg[2] = {0}; // msg[0] = nbTasks, msg[1] = offset
	int rank;
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	while (1){
		MPI_Recv(msg, 2, MPI_LONG_INT, MPI_ANY_SOURCE, ASK4TASKS, MPI_COMM_WORLD, &status);
		
		printf("> [%d] Received request from %d\n", rank, status.MPI_SOURCE);
		
		// We take half of the remaining tasks
		pthread_mutex_lock(&tasksMutex);
		
		if (t->curTask + t->offset != t->finalTask){
			msg[0] = (t->finalTask - (t->curTask + t->offset)) / 2;
			msg[1] = t->finalTask - msg[0];
			t->finalTask = msg[1] - 1;
		}
		// Tasks copy
		// TODO check return
		
		printf("> [%d] Sending %d tasks to %d\n", rank, msg[0], status.MPI_SOURCE);
		pthread_mutex_unlock(&tasksMutex);


		MPI_Send(msg, 2, MPI_LONG_INT, status.MPI_SOURCE, TASKS_ANNOUNCEMENT, MPI_COMM_WORLD);
		if (msg[0] > 0){
			MPI_Send(&(t->bound[t->finalTask + 1]), (4 * msg[0]), MPI_DOUBLE, status.MPI_SOURCE, TASKS_SENDING, MPI_COMM_WORLD);
		}

	}
}
