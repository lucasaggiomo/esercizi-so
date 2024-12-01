#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>

#include "messaggi.h"

struct shared {
    struct msg_richiesta* buffer;
    int testa;
    int coda;
    int count;
    int dim;

    pthread_mutex_t mutex;
    pthread_cond_t cv_produttore;
    pthread_cond_t cv_consumatore;
};

void init_shared(struct shared* sh, int dim);
void destroy_shared(struct shared* sh);

void produci(struct shared* sh, struct msg_richiesta* input);
void consuma(struct shared* sh, struct msg_richiesta* output);

#endif