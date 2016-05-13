#include <cmath>
#include "buffer.h"

extern int routine_complexity;

void consuming_routine(chunk_t *chunk) {

    double count = 1;

    for (int i = 0; i < routine_complexity; ++i) {
        count += sin(count);
    }

}

