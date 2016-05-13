#include "libmito.h"

#ifdef VTUNE_LABELS
__itt_domain *domain;
__itt_string_handle **io_tasks;
__itt_string_handle *processing_task;
__itt_string_handle *output_task;
#endif

chunk_t *mem_chunks;
chunk_buffer_t empty_buffer;
chunk_buffer_t processable_buffer;
chunk_buffer_t writable_buffer;
pthread_mutex_t thread_id_mutex;
pthread_barrier_t start_barrier;
atomic_val_t next_offset;
atomic_val_t num_chunk_read;
atomic_val_t num_chunk_comp;
atomic_val_t num_chunk_writ;
atomic_val_t num_chunk_read_pushed;
atomic_val_t num_chunk_comp_pushed;
atomic_val_t num_chunk_writ_pushed;
char *in_file_name, *out_file_name;

void (*routine)(chunk_t *chunk);

int __DEBUG = 0;

void set_routine(void (*f)(chunk_t *chunk)) {
    routine = f;
}

void wait(int usecond) {
	timeval tm_wait;
	tm_wait.tv_sec = usecond / 1000000;
	tm_wait.tv_usec = usecond % 1000000;
	select(1, NULL, NULL, NULL, &tm_wait);
}

double calc_diff(timeval& end, timeval& start) {
    return ((end.tv_sec - start.tv_sec) * 1000000u +
            end.tv_usec - start.tv_usec) / 1.e6;
}

chunk_t *pop_chunk(chunk_buffer_t &buffer) {
	chunk_t *chunk = NULL;
	pthread_mutex_lock(&buffer.lock);
	if (buffer.buf.size()) {
		chunk = buffer.buf.front();
		buffer.buf.pop();
	}
	pthread_mutex_unlock(&buffer.lock);
	return chunk;
}

void push_chunk(chunk_buffer_t &buffer, chunk_t *chunk) {
	pthread_mutex_lock(&buffer.lock);
	buffer.buf.push(chunk);
	pthread_mutex_unlock(&buffer.lock);
}

void *thread_work(void *data) {
	int *id = (int*) data;

	// thread 0 is the I/O thread
	if (*id == 0) {
		// allocate memory for all the chunks
		mem_chunks = new chunk_t[NUM_CHUNKS];

		// assign all chunks to the empty_buffer
		for (int i = 0; i < NUM_CHUNKS; ++i) {
            mem_chunks[i].id = i;
			mem_chunks[i].terminal = false;
			empty_buffer.buf.push(&mem_chunks[i]);
		}

	}

	// everyone synchronize
	pthread_barrier_wait(&start_barrier);

    long unsigned thread_waits = 0;
	timeval start, end;

    if (*id < NUM_INP_THREADS) {		// early thread ids - input

        FILE* infile;
        // try to open the file
        if ((infile = fopen(in_file_name, "r")) == NULL) {
            printf("\rfile %s not found or cannot be opened.\n", in_file_name);
            exit(0);
        }
        printf("\rthread %d doing input.\n", *id);

        // now keep reading from file as long as we can lock empty_chunks
        int num_bytes_read;
        double io_time = 0;
        do {

            // wait till you can get a new empty chunk
            chunk_t *chunk;
            while (!empty_buffer.closed &&
                    (chunk = pop_chunk(empty_buffer)) == NULL) {
                ++thread_waits;
                wait(SLEEP_DUR);
            }
            if (empty_buffer.closed) {
                break;
            }

            num_chunk_read.lock();
            chunk->rank = num_chunk_read.value++;
            num_chunk_read.unlock();

            // read a chunk from file system
#ifdef VTUNE_LABELS
            __itt_task_begin (domain, __itt_null, __itt_null, io_tasks[chunk->id]);
#endif
			gettimeofday(&start, NULL);
            fseek(infile, next_offset.value, SEEK_SET);
            next_offset.increment(sizeof(CHUNK_TYPE) * CHUNK_SIZE);
            num_bytes_read = fread(chunk->data, sizeof(CHUNK_TYPE), CHUNK_SIZE, infile);
            chunk->size = num_bytes_read;
            gettimeofday(&end, NULL);
            io_time += calc_diff(end, start);
#ifdef VTUNE_LABELS
            __itt_task_end (domain);
#endif
            chunk->terminal = num_bytes_read < CHUNK_SIZE;
            if (__DEBUG > 2)
            printf("\rthread %d read chunk #%d, size %lld bytes.\n", *id, chunk->id, chunk->size);

            // make sure the chunk is pushed in order (number of chunks pushed is less than your id)
            // push the chunk (now processable) to the processable queue
            while (chunk->rank != num_chunk_read_pushed.value) {
                wait(SLEEP_DUR);
            }
            push_chunk(processable_buffer, chunk);
            num_chunk_read_pushed.increment(1);

        } while (num_bytes_read == CHUNK_SIZE);

        // if it's end of line, close the buffer
        empty_buffer.closed = true;
        printf("\rfinished reading file. Input I/O time: %f.\n", io_time);
        if (__DEBUG > 2)
        printf("\rread %ld chunks.\n", num_chunk_read.value);

    }

    // ----------------------------------------------------------------------

    // middle thread ids - processing
    else if (*id >= NUM_INP_THREADS && *id < NUM_INP_THREADS + NUM_COMP_THREADS) {

        double proc_time = 0;
        printf("\rthread %d doing compute.\n", *id);

        chunk_t *chunk;
        do {
            // wait till you can lock a processable chunk
            while (!processable_buffer.closed &&
                    (chunk = pop_chunk(processable_buffer)) == NULL) {
                ++thread_waits;
                wait(SLEEP_DUR);
            }
            if (processable_buffer.closed) {
                break;
            }
            num_chunk_comp.increment(1);

#ifdef VTUNE_LABELS
            __itt_task_begin (domain, __itt_null, __itt_null, processing_task);
#endif
            gettimeofday(&start, NULL);
            routine(chunk);
            gettimeofday(&end, NULL);
            proc_time += calc_diff(end, start);
#ifdef VTUNE_LABELS
            __itt_task_end (domain);
#endif

            printf("\rprocessed chunk #%d.", chunk->rank);

            // make sure chunks are also pushed in order on the processable queue
            while (chunk->rank != num_chunk_comp_pushed.value) {
                wait(SLEEP_DUR);
            }
            if (NUM_OUTP_THREADS > 0)
                push_chunk(writable_buffer, chunk);
            else
                push_chunk(empty_buffer, chunk);

            num_chunk_comp_pushed.increment(1);
            if (__DEBUG > 2)
                printf("\rprocessed chunk #%d, total processed %ld.\n", chunk->id, num_chunk_comp.value);
        } while(num_chunk_comp.value < num_chunk_read.value || !empty_buffer.closed);

        processable_buffer.closed = true;
        printf("\rfinished processing file. Processing time: %f.\n", proc_time);
    }

    // ----------------------------------------------------------------------
    
    else {                      // late thread ids - output

        printf("\rthread %d doing output.\n", *id);
        FILE* outfile;
        double io_time = 0;

        // try to open the file
        if ((outfile = fopen(out_file_name, "w")) == NULL) {
            printf("\rfile %s cannot be opened.\n", out_file_name);
            exit(0);
        }

        chunk_t *chunk;
        do {
            // wait till you can get a new empty chunk
            while (!writable_buffer.closed &&
                    (chunk = pop_chunk(writable_buffer)) == NULL) {
                ++thread_waits;
                wait(SLEEP_DUR);
            }
            if (writable_buffer.closed) {
                break;
            }
            num_chunk_writ.increment(1);

            // write the chunk to file system
#ifdef VTUNE_LABELS
            __itt_task_begin (domain, __itt_null, __itt_null, output_task);
#endif
			gettimeofday(&start, NULL);
            fwrite(chunk->data, sizeof(CHUNK_TYPE), chunk->size, outfile);
            gettimeofday(&end, NULL);
            io_time += calc_diff(end, start);
#ifdef VTUNE_LABELS
            __itt_task_end (domain);
#endif
            // push the chunk (now processable) to the processable queue
            // make sure chunks are also pushed in order on the processable queue
            while (chunk->rank != num_chunk_writ_pushed.value) {
                wait(SLEEP_DUR);
            }
            push_chunk(empty_buffer, chunk);
            num_chunk_writ_pushed.increment(1);

            if (__DEBUG > 2)
            printf("\rwrote chunk #%d, size %lld bytes, total written %ld.\n", chunk->id, chunk->size, num_chunk_writ.value);
        } while(num_chunk_writ.value < num_chunk_read.value || !processable_buffer.closed);

        writable_buffer.closed = true;
        printf("\rfinished writing the file. Output I/O time: %f.\n", io_time);

    }

    return NULL;
}

void init(char* in_file, char* out_file) {

    // validate input and output file paths
    FILE *fd;
    if ((fd= fopen(in_file, "r")) == NULL) {
        printf("\rinput file %s not found or cannot be opened.\n", in_file);
        exit(0);
    }
    if ((fd= fopen(out_file, "w")) == NULL) {
        printf("\routput file %s cannot be created. check permissions.\n", out_file);
        exit(0);
    }


#ifdef VTUNE_LABELS
    domain = __itt_domain_create("Task Domain");
    io_tasks = new __itt_string_handle*[NUM_CHUNKS];
    for (int i = 0; i < NUM_CHUNKS; ++i) {
        char name[4] = "IO";
        name[2] = (char)48+i;
        name[3] = '\0';
        io_tasks[i] = __itt_string_handle_create(name);
    }
    processing_task = __itt_string_handle_create("Processing");
    output_task = __itt_string_handle_create("Output");
#endif

    pthread_t threads[NUM_THREADS - 1];
	int thread_ids[NUM_THREADS];
	thread_ids[0] = 0;
    in_file_name = in_file;
    out_file_name = out_file;

	pthread_mutex_init(&empty_buffer.lock, NULL);
	pthread_mutex_init(&processable_buffer.lock, NULL);

	// initialize barrier
	pthread_barrier_init(&start_barrier, NULL, NUM_THREADS);

	for (int i = 1; i < NUM_THREADS; ++i) {
		thread_ids[i] = i;
		pthread_create(&threads[i - 1], NULL, thread_work,
				(void*) &thread_ids[i]);
	}

	// execute the thread_work function like other threads
	thread_work((void*) &thread_ids[0]);

	void *status;
	for (int i = 1; i < NUM_THREADS; ++i) {
		pthread_join(threads[i - 1], &status);
	}

}
