#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"

void init_buffer(struct buffer* b) {
    b->ID = counter++;

    b->num_cv_lettori = 0;
    b->num_cv_scrittori = 0;
    b->num_lettori = 0;
    b->num_scrittori = 0;

    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cv_scrittori, NULL);
    pthread_cond_init(&b->cv_lettori, NULL);

    b->num = 0;
}

void destroy_buffer(struct buffer* b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cv_scrittori);
    pthread_cond_destroy(&b->cv_lettori);
}

void scrivi(struct buffer* b, int new_num) {
    inizio_scrittura(b);

    b->num = new_num;

    fine_scrittura(b);
}

int leggi(struct buffer* b) {
    int num;

    inizio_lettura(b);

    num = b->num;

    fine_lettura(b);

    return num;
}

void inizio_lettura(struct buffer* b) {
    pthread_mutex_lock(&b->mutex);

    while (b->num_scrittori > 0) {
        b->num_cv_lettori++;
        pthread_cond_wait(&b->cv_lettori, &b->mutex);
        b->num_cv_lettori--;
    }

    b->num_lettori++;

    pthread_mutex_unlock(&b->mutex);
}
void fine_lettura(struct buffer* b) {
    pthread_mutex_lock(&b->mutex);

    b->num_lettori--;
    if (b->num_lettori == 0) {
        pthread_cond_signal(&b->cv_scrittori);
    }

    pthread_mutex_unlock(&b->mutex);
}

void inizio_scrittura(struct buffer* b) {
    pthread_mutex_lock(&b->mutex);

    while (b->num_scrittori > 0 || b->num_lettori > 0) {
        b->num_cv_scrittori++;
        pthread_cond_wait(&b->cv_scrittori, &b->mutex);
        b->num_cv_scrittori--;
    }

    b->num_scrittori++;

    pthread_mutex_unlock(&b->mutex);
}
void fine_scrittura(struct buffer* b) {
    pthread_mutex_lock(&b->mutex);

    b->num_scrittori--;

    if (b->num_cv_scrittori > 0) {
        pthread_cond_signal(&b->cv_scrittori);
    } else if (b->num_cv_lettori > 0) {
        pthread_cond_broadcast(&b->cv_lettori);
    }

    pthread_mutex_unlock(&b->mutex);
}