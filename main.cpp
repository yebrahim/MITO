#include "libmito.h"
#include "routine.cpp"

int routine_complexity = 10000000;
int NUM_CHUNKS = 10;
int NUM_INP_THREADS = 1;
int NUM_COMP_THREADS = 1;
int NUM_OUTP_THREADS = 1;
int NUM_THREADS;

int main(int argc, char **argv) {

    if (argc < 3) {
        printf("Usage: %s in_file out_file [#in_threads #compute_threads #out_threads]\n", argv[0]);
        return 0;
    }

    if (argc > 3) {

        if (argc < 6) {
            printf("Usage: %s in_file out_file [#in_threads #compute_threads #out_threads]\n", argv[0]);
            return 0;
        }

        NUM_INP_THREADS = atoi(argv[3]);
        NUM_COMP_THREADS = atoi(argv[4]);
        NUM_OUTP_THREADS = atoi(argv[5]);

    }

    NUM_THREADS = NUM_INP_THREADS + NUM_COMP_THREADS + NUM_OUTP_THREADS;
    set_routine(&consuming_routine);
    init(argv[1], argv[2]);

	return 0;
}

