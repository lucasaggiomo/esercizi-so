#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wrapper.h"

#include "shared.h"

void init_shared(struct shared* sh, int dim) {
    sh->coda = 0;
    sh->testa = 0;
    sh->count = 0;

    sh->dim = dim;
    sh->buffer = Malloc(dim * sizeof(*sh->buffer));

    pthread_mutex_init(&sh->mutex, NULL);
    pthread_cond_init(&sh->cv_produttore, NULL);
    pthread_cond_init(&sh->cv_consumatore, NULL);
}

void destroy_shared(struct shared* sh) {
    pthread_mutex_destroy(&sh->mutex);
    pthread_cond_destroy(&sh->cv_produttore);
    pthread_cond_destroy(&sh->cv_consumatore);

    free(sh->buffer);
}

void produci(struct shared* sh, struct msg_richiesta* input) {
    pthread_mutex_lock(&sh->mutex);

    while (sh->count == sh->dim) {
        pthread_cond_wait(&sh->cv_produttore, &sh->mutex);
    }

    memcpy(&sh->buffer[sh->testa], input, sizeof(struct msg_richiesta));
    sh->testa = (sh->testa + 1) % sh->dim;
    sh->count++;

    pthread_cond_signal(&sh->cv_consumatore);

    pthread_mutex_unlock(&sh->mutex);
}

int produci_se_puoi(struct shared* sh, struct msg_richiesta* input) {
    pthread_mutex_lock(&sh->mutex);

    if (sh->count == sh->dim) {
        pthread_mutex_unlock(&sh->mutex);
        return 0;
    }

    memcpy(&sh->buffer[sh->testa], input, sizeof(struct msg_richiesta));
    sh->testa = (sh->testa + 1) % sh->dim;
    sh->count++;

    pthread_cond_signal(&sh->cv_consumatore);

    pthread_mutex_unlock(&sh->mutex);

    return 1;
}

void consuma(struct shared* sh, struct msg_richiesta* output) {
    pthread_mutex_lock(&sh->mutex);

    printf("[Mittente] Provo a consumare\n");
    while (sh->count == 0) {
        printf("[Mittente] Attendo per il consumo...\n");
        pthread_cond_wait(&sh->cv_consumatore, &sh->mutex);
    }
    printf("[Mittente] Effettuo il consumo\n");

    memcpy(output, &sh->buffer[sh->coda], sizeof(struct msg_richiesta));
    sh->coda = (sh->coda + 1) % sh->dim;
    sh->count--;

    pthread_cond_signal(&sh->cv_produttore);

    pthread_mutex_unlock(&sh->mutex);
}
