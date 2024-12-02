#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "queue.h"

struct semaphore {
    int value;
    struct queue queue;
};

void init_semaphore(struct semaphore* s, int queueCapacity, int startingValue);
void destroy_semaphore(struct semaphore* s);

void wait_semaphore(struct semaphore* s, int tid);
void signal_semaphore(struct semaphore* s);

#endif