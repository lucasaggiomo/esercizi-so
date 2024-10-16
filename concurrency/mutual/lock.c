#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define N 2  // numero di processi

static volatile lock;

void enter_region(int process) {
    while (lock == 1)
        /* WAIT */;
    lock = 1;
}
void leave_region(int process) {
    lock = 0;
}

#define ITERATIONS 10000000

static volatile int counter;
void critical_region() {
    counter++;
}

void* do_stuff(void* arg) {
    int process = *(int*)arg;
    printf("%d: begin\n", process);
    for (int i = 0; i < ITERATIONS; i++) {
        enter_region(process);
        critical_region();
        leave_region(process);
    }
    printf("%d: done\n", process);
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
    lock = 0;

    pthread_t p1, p2;
    int p1_id = 0;
    int p2_id = 1;

    counter = 0;

    printf("main: begin (counter = %d)\n", counter);

    Pthread_create(&p1, NULL, do_stuff, &p1_id);
    Pthread_create(&p2, NULL, do_stuff, &p2_id);

    // join waits for the threads to finish
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    printf("main: done with both (counter = %d)\n", counter);
}
