#include "buffer.h"

chunk_buffer_t::chunk_buffer_t() {
    closed = false;
}

atomic_val_t::atomic_val_t() {
    value = 0;
}

void
atomic_val_t::increment (int64_t inc) {
    pthread_mutex_lock(&lock_mutex);
    value += inc;
    pthread_mutex_unlock(&lock_mutex);
}

void
atomic_val_t::lock() {
    pthread_mutex_lock(&lock_mutex);
}

void
atomic_val_t::unlock() {
    pthread_mutex_unlock(&lock_mutex);
}
