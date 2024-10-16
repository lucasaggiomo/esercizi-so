#define _GNU_SOURCE
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "semaphore.h"

#define N 2  // numero di processi

static struct semaphore s;

void enter_region(int tid) {
    wait_semaphore(&s, tid);
}
void leave_region(int tid) {
    signal_semaphore(&s);
}

#define ITERATIONS 10000000

static volatile int counter;
void critical_region() {
    counter++;
}

void* do_stuff(void* arg) {
    pid_t tid = gettid();
    printf("%d: begin\n", tid);
    for (int i = 0; i < ITERATIONS; i++) {
        enter_region(tid);
        critical_region();
        leave_region(tid);
    }
    printf("%d: done\n", tid);
    return NULL;
}

void Pthread_create(pthread_t* thread,
                    const pthread_attr_t* attr,
                    void* (*start_routine)(void*),
                    void* arg) {
    if (pthread_create(thread, attr, start_routine, arg)) {
        perror("Error in thread creation");
        exit(EXIT_FAILURE);
    }
}

void Pthread_join(pthread_t thread, void** retval) {
    if (pthread_join(thread, retval)) {
        perror("Error in thread join");
        exit(EXIT_FAILURE);
    }
}

int main() {
    init_semaphore(&s, N, 1);

    pthread_t p1, p2;

    counter = 0;

    printf("main: begin (counter = %d)\n", counter);

    Pthread_create(&p1, NULL, do_stuff, NULL);
    Pthread_create(&p2, NULL, do_stuff, NULL);

    // join waits for the threads to finish
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    printf("main: done with both (counter = %d)\n", counter);

    destroy_semaphore(&s);
}
