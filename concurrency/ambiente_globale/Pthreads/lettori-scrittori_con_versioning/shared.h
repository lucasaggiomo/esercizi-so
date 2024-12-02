#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>

struct shared {
    int data;
    int versione;

    int num_lettori;
    int num_scrittori;

    int num_cv_lettori;
    int num_cv_scrittori;
    pthread_mutex_t mutex;
    pthread_cond_t cv_lettori;
    pthread_cond_t cv_scrittori;
};

void init_shared(struct shared* sh);
void destroy_shared(struct shared* sh);

void inizia_lettura(struct shared* sh, int* versione);

void fine_lettura(struct shared* sh);

int leggi(struct shared* sh, int* versione);

void inizia_scrittura(struct shared* sh);

void fine_scrittura(struct shared* sh);

int scrivi(struct shared* sh, int input);

#endif