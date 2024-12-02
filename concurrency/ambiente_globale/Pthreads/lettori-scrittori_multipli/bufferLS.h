#ifndef BUFFERLS_H
#define BUFFERLS_H

#include <pthread.h>

struct bufferLS {
    int data;
    int num_lettori;

    pthread_mutex_t mutex;
    pthread_cond_t cv_scrittori;
};

void init_buffer(struct bufferLS* b);
void destroy_buffer(struct bufferLS* b);

int leggi(struct bufferLS* b);
void scrivi(struct bufferLS* b, int input);

#endif