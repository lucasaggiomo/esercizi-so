#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer_stato.h"

void init_buffer(struct buffer_stato_m* m, int dim, size_t element_size) {
    m->vettore_buffer = malloc(dim * element_size);
    if (!m->vettore_buffer) {
        perror("Errore nella malloc");
        exit(1);
    }

    m->vettore_stato = malloc(dim * sizeof(enum stato));
    if (!m->vettore_stato) {
        perror("Errore nella malloc");
        exit(1);
    }
    for (int i = 0; i < dim; i++) {
        m->vettore_stato[i] = LIBERO;
    }
    // memset(m->vettore_stato, LIBERO, dim * sizeof(enum stato));

    m->num_liberi = dim;
    m->num_occupati = 0;

    m->dim = dim;
    m->element_size = element_size;

    pthread_mutex_init(&m->mutex, NULL);
    pthread_cond_init(&m->cv_produttore, NULL);
    pthread_cond_init(&m->cv_consumatore, NULL);
}

void destroy_buffer(struct buffer_stato_m* m) {
    free(m->vettore_buffer);
    free(m->vettore_stato);

    pthread_mutex_destroy(&m->mutex);
    pthread_cond_destroy(&m->cv_produttore);
    pthread_cond_destroy(&m->cv_consumatore);
}

void* item_at(struct buffer_stato_m* m, int index) {
    return (byte*)m->vettore_buffer + index * m->element_size;
}

int first_index_of(struct buffer_stato_m* m, enum stato stato) {
    int index = 0;
    while (m->vettore_stato[index] != stato && index < m->dim) {
        index++;
    }
    return index == m->dim ? -1 : index;
}

void produci_buffer(struct buffer_stato_m* m, void* item) {
    if (!item) {
        fprintf(stderr, "Errore: l'elemento da produrre era NULL\n");
        exit(1);
    }

    int index = 0;

    pthread_mutex_lock(&m->mutex);

    while (m->num_liberi == 0) {
        pthread_cond_wait(&m->cv_produttore, &m->mutex);
    }

    index = first_index_of(m, LIBERO);
    if (index == -1) {
        fprintf(stderr, "Errore: nessun elemento libero trovato\n");
        exit(1);
    }

    m->vettore_stato[index] = IN_USO;
    m->num_liberi--;

    pthread_mutex_unlock(&m->mutex);

    memcpy(item_at(m, index), item, m->element_size);

    pthread_mutex_lock(&m->mutex);

    m->vettore_stato[index] = OCCUPATO;
    m->num_occupati++;

    pthread_cond_signal(&m->cv_consumatore);

    pthread_mutex_unlock(&m->mutex);
}

void consuma_buffer(struct buffer_stato_m* m, void* output) {
    if (!output) {
        fprintf(stderr, "Errore: l'elemento su cui produrre era NULL\n");
        exit(1);
    }

    int index = 0;

    pthread_mutex_lock(&m->mutex);

    while (m->num_occupati == 0) {
        pthread_cond_wait(&m->cv_consumatore, &m->mutex);
    }

    index = first_index_of(m, OCCUPATO);
    if (index == -1) {
        fprintf(stderr, "Errore: nessun elemento occupato trovato\n");
        exit(1);
    }

    m->vettore_stato[index] = IN_USO;
    m->num_occupati--;

    pthread_mutex_unlock(&m->mutex);

    memcpy(output, item_at(m, index), m->element_size);

    pthread_mutex_lock(&m->mutex);

    m->vettore_stato[index] = LIBERO;
    m->num_liberi++;

    pthread_cond_signal(&m->cv_produttore);

    pthread_mutex_unlock(&m->mutex);
}