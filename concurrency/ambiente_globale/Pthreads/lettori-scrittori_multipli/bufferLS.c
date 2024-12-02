#include "bufferLS.h"
#include "wrapper.h"

void init_buffer(struct bufferLS* b) {
    b->num_lettori = 0;

    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cv_scrittori, NULL);
}

void destroy_buffer(struct bufferLS* b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cv_scrittori);
}

int leggi(struct bufferLS* b) {
    int output;

    pthread_mutex_lock(&b->mutex);

    b->num_lettori++;

    pthread_mutex_unlock(&b->mutex);

    output = b->data;

    pthread_mutex_lock(&b->mutex);

    b->num_lettori--;

    pthread_mutex_unlock(&b->mutex);

    return output;
}

void scrivi(struct bufferLS* b, int v) {
    pthread_mutex_lock(&b->mutex);

    while (b->num_lettori > 0) {
        pthread_cond_wait(&b->cv_scrittori, &b->mutex);
    }

    b->data = v;

    pthread_mutex_unlock(&b->mutex);
}