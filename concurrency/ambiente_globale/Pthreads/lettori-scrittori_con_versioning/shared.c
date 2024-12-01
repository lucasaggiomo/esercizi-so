#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wrapper.h"

#include "shared.h"

void init_shared(struct shared* sh) {
    sh->data = 0;
    sh->versione = 0;

    sh->num_lettori = 0;
    sh->num_scrittori = 0;
    sh->num_cv_lettori = 0;
    sh->num_cv_scrittori = 0;

    pthread_mutex_init(&sh->mutex, NULL);
    pthread_cond_init(&sh->cv_lettori, NULL);
    pthread_cond_init(&sh->cv_scrittori, NULL);
}

void destroy_shared(struct shared* sh) {
    pthread_mutex_destroy(&sh->mutex);
    pthread_cond_destroy(&sh->cv_lettori);
    pthread_cond_destroy(&sh->cv_scrittori);
}

void inizia_lettura(struct shared* sh, int* versione) {
    pthread_mutex_lock(&sh->mutex);

    while (sh->num_scrittori > 0 || sh->versione == *versione) {
        sh->num_cv_lettori++;
        pthread_cond_wait(&sh->cv_lettori, &sh->mutex);
        sh->num_cv_lettori--;
    }

    sh->num_lettori++;

    pthread_mutex_unlock(&sh->mutex);
}

void fine_lettura(struct shared* sh) {
    pthread_mutex_lock(&sh->mutex);

    sh->num_lettori--;

    if (sh->num_lettori == 0) {
        pthread_cond_signal(&sh->cv_scrittori);
    }

    pthread_mutex_unlock(&sh->mutex);
}

int leggi(struct shared* sh, int* versione) {
    int output;

    inizia_lettura(sh, versione);

    output = sh->data;
    *versione = sh->versione;

    fine_lettura(sh);

    return output;
}

void inizia_scrittura(struct shared* sh) {
    pthread_mutex_lock(&sh->mutex);

    while (sh->num_scrittori > 0 || sh->num_lettori > 0) {
        sh->num_cv_scrittori++;
        pthread_cond_wait(&sh->cv_scrittori, &sh->mutex);
        sh->num_cv_scrittori--;
    }

    sh->num_scrittori++;

    pthread_mutex_unlock(&sh->mutex);
}

void fine_scrittura(struct shared* sh) {
    pthread_mutex_lock(&sh->mutex);

    sh->num_scrittori--;

    if (sh->num_cv_scrittori > 0) {
        pthread_cond_signal(&sh->cv_scrittori);
    } else if (sh->num_cv_lettori > 0) {
        pthread_cond_signal(&sh->cv_lettori);
    }

    pthread_mutex_unlock(&sh->mutex);
}

int scrivi(struct shared* sh, int input) {
    inizia_scrittura(sh);

    sh->data = input;
    int versione = ++(sh->versione);

    fine_scrittura(sh);

    return versione;
}