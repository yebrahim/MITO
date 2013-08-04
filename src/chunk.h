#ifndef CHUNK_H
#define CHUNK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
using namespace std;

// CHUNK_TYPE specifies the type of one element in the chunk
// use native types with well-specified widths (for example avoid int)
typedef double CHUNK_TYPE;

// the size of a chunk. every input thread will read at most CHUNK_SIZE
// bytes from disk and pass a pointer to the first element to the user
// routine. using a power of two is better to align with the disk buffer
static const long long CHUNK_SIZE = 32 * 1024 * 1024;

struct chunk_t {
    // chunk data
	CHUNK_TYPE data[CHUNK_SIZE];

    // number of CHUNK_TYPE elements contained in this chunk
    long long size;

    // the terminal chunk is the last chunk read from file
	bool	terminal;

    // unique id for debugging purposes
    short   id;

    // the chunk's rank is its order in the read queue
    // it's used to keep chunks always in the same order on all queues
    short   rank;
};

#endif
