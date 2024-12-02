#ifndef SHARED_H
#define SHARED_H

#include "item.h"
#include <pthread.h>

struct buffer_m {
    struct item buffer;
    int num_lettori;
    int num_scrittori;
    int num_cv_lettori;
    int num_cv_scrittori;

    pthread_mutex_t mutex;
    pthread_cond_t cv_lettore;
    pthread_cond_t cv_scrittore;
};

void init_buffer(struct buffer_m* b);
void destroy_buffer(struct buffer_m* b);

void aggiorna(struct buffer_m* b, struct item e);
struct item consulta(struct buffer_m* b);

#endif