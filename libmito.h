#ifndef LIBMITO_H
#define LIBMITO_H

#include "buffer.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>

#ifdef VTUNE_LABELS
#include "ittnotify.h"
extern __itt_domain *domain;
extern __itt_string_handle **io_tasks;
extern __itt_string_handle *processing_task;
extern __itt_string_handle *output_task;
#endif

extern int NUM_CHUNKS;
extern int NUM_INP_THREADS;
extern int NUM_COMP_THREADS;
extern int NUM_OUTP_THREADS;
extern int NUM_THREADS;

extern chunk_t *mem_chunks;
extern chunk_buffer_t empty_buffer;
extern chunk_buffer_t processable_buffer;
extern chunk_buffer_t writable_buffer;
extern pthread_mutex_t thread_id_mutex;
extern pthread_barrier_t start_barrier;
extern atomic_val_t next_offset;
extern atomic_val_t num_chunk_read;
extern atomic_val_t num_chunk_comp;
extern atomic_val_t num_chunk_writ;
extern atomic_val_t num_chunk_read_pushed;
extern atomic_val_t num_chunk_comp_pushed;
extern atomic_val_t num_chunk_writ_pushed;
extern char *in_file_name, *out_file_name;

extern void (*routine)(chunk_t *chunk);

void set_routine(void (*f)(chunk_t *chunk));

void wait(int usecond);

double calc_diff(timeval& end, timeval& start);

chunk_t *pop_chunk(chunk_buffer_t &buffer);

void push_chunk(chunk_buffer_t &buffer, chunk_t *chunk);

void *thread_work(void *data);

void init(char* in_file, char* out_file);

extern int __DEBUG;

#endif
