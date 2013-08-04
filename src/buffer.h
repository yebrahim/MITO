#ifndef BUFFER_H
#define BUFFER_H

#include "chunk.h"
#include <pthread.h>
#include <queue>

static const int SLEEP_DUR = 1024;

class chunk_buffer_t {
public:
	queue<chunk_t*> buf;
	pthread_mutex_t lock;
    bool closed;
    chunk_buffer_t();
};

class atomic_val_t {
private:
    pthread_mutex_t lock_mutex;
public:
    int64_t value;
    atomic_val_t();
    void increment(int64_t inc);
    void lock();
    void unlock();
};

#endif
