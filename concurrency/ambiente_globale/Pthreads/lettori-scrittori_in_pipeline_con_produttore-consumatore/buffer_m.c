#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wrapper.h"

#include "buffer_m.h"

void init_buffer(struct buffer_stato_m* b) {
    b->num_lettori = 0;
    b->num_scrittori = 0;
    b->num_cv_lettori = 0;
    b->num_cv_scrittori = 0;

    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cv_lettore, NULL);
    pthread_cond_init(&b->cv_scrittore, NULL);
}

void destroy_buffer(struct buffer_stato_m* b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cv_lettore);
    pthread_cond_destroy(&b->cv_scrittore);
}

void aggiorna(struct buffer_stato_m* b, struct item e) {
    pthread_mutex_lock(&b->mutex);

    while (b->num_cv_scrittori > 0 || b->num_lettori > 0) {
        b->num_cv_scrittori++;
        pthread_cond_wait(&b->cv_scrittore, &b->mutex);
        b->num_cv_scrittori--;
    }

    b->num_scrittori++;

    pthread_mutex_unlock(&b->mutex);

    b->buffer = e;

    pthread_mutex_lock(&b->mutex);

    b->num_scrittori--;

    if (b->num_cv_scrittori > 0) {
        pthread_cond_signal(&b->cv_scrittore);
    } else if (b->num_cv_lettori > 0) {
        pthread_cond_broadcast(&b->cv_lettore);
    }

    pthread_mutex_unlock(&b->mutex);
}

struct item consulta(struct buffer_stato_m* b) {
    struct item e;

    pthread_mutex_lock(&b->mutex);

    while (b->num_scrittori > 0) {
        b->num_cv_lettori++;
        pthread_cond_wait(&b->cv_lettore, &b->mutex);
        b->num_cv_lettori--;
    }

    b->num_lettori++;

    pthread_mutex_unlock(&b->mutex);

    e = b->buffer;

    pthread_mutex_lock(&b->mutex);

    b->num_lettori--;

    if (b->num_lettori == 0) {
        pthread_cond_signal(&b->cv_scrittore);
    }

    pthread_mutex_unlock(&b->mutex);

    return e;
}