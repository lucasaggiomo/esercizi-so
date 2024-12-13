#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>

static int counter;

struct buffer {
    int ID;
    int num;

    int num_lettori;
    int num_scrittori;
    int num_cv_lettori;
    int num_cv_scrittori;

    pthread_mutex_t mutex;
    pthread_cond_t cv_lettori;
    pthread_cond_t cv_scrittori;
};

void init_buffer(struct buffer* b);
void destroy_buffer(struct buffer* b);

void scrivi(struct buffer* b, int new_num);
int leggi(struct buffer* b);

void inizio_lettura(struct buffer* b);
void fine_lettura(struct buffer* b);

void inizio_scrittura(struct buffer* b);
void fine_scrittura(struct buffer* b);

#endif