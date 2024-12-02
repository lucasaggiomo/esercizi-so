#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wrapper.h"

#include "vettore_m.h"

void init_vettore(struct vettore_m* v, int dim) {
    v->num_pieni = 0;
    v->num_vuoti = dim;
    v->dim = dim;
    v->vettore = Malloc(dim * sizeof(struct item));
    v->stato = Malloc(dim * sizeof(enum stato));
    for (int i = 0; i < dim; i++) {
        v->stato[i] = VUOTO;
    }
    // memset(v->stato, VUOTO, dim * sizeof(enum stato));

    pthread_mutex_init(&v->mutex, NULL);
    pthread_cond_init(&v->cv_produttore, NULL);
    pthread_cond_init(&v->cv_consumatore, NULL);
}

void destroy_vettore(struct vettore_m* v) {
    pthread_mutex_destroy(&v->mutex);
    pthread_cond_destroy(&v->cv_produttore);
    pthread_cond_destroy(&v->cv_consumatore);

    free(v->vettore);
    free(v->stato);
}

void genera(struct vettore_m* v, struct item e) {
    int index = 0;

    pthread_mutex_lock(&v->mutex);

    while (v->num_vuoti == 0) {
        pthread_cond_wait(&v->cv_produttore, &v->mutex);
    }

    while (v->stato[index] != VUOTO && index < v->dim) {
        index++;
    }

    v->stato[index] = IN_USO;
    v->num_vuoti--;

    pthread_mutex_unlock(&v->mutex);

    v->vettore[index] = e;

    pthread_mutex_lock(&v->mutex);

    v->stato[index] = PIENO;
    v->num_pieni++;

    pthread_cond_signal(&v->cv_consumatore);

    pthread_mutex_unlock(&v->mutex);
}

struct item preleva(struct vettore_m* v) {
    struct item e;

    int index = 0;

    pthread_mutex_lock(&v->mutex);

    while (v->num_pieni == 0) {
        pthread_cond_wait(&v->cv_consumatore, &v->mutex);
    }

    while (v->stato[index] != PIENO && index < v->dim) {
        index++;
    }

    v->stato[index] = IN_USO;
    v->num_pieni--;

    pthread_mutex_unlock(&v->mutex);

    e = v->vettore[index];

    pthread_mutex_lock(&v->mutex);

    v->stato[index] = VUOTO;
    v->num_vuoti++;

    pthread_cond_signal(&v->cv_produttore);

    pthread_mutex_unlock(&v->mutex);

    return e;
}
