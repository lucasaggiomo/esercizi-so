#include <string.h>

#include "bufchar.h"

void inizializza(struct BufChar* b) {
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cv_produttore, NULL);
    pthread_cond_init(&b->cv_consumatore, NULL);

    b->num_occupati = 0;
}
void distruggi(struct BufChar* b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cv_produttore);
    pthread_cond_destroy(&b->cv_consumatore);
}

void produci(struct BufChar* b, const char* caratteri, int num_char) {
    pthread_mutex_lock(&b->mutex);

    while (DIM_BUFFER - b->num_occupati < num_char) {
        pthread_cond_wait(&b->cv_produttore, &b->mutex);
    }

    strncpy(b->buffer + b->num_occupati, caratteri, num_char);
    b->num_occupati += num_char;

    pthread_cond_signal(&b->cv_consumatore);

    pthread_mutex_unlock(&b->mutex);
}

// ritorna numero_caratteri consumati
int consuma(struct BufChar* b, char* caratteri) {
    int num_liberati;

    pthread_mutex_lock(&b->mutex);

    while (b->num_occupati == 0) {
        pthread_cond_wait(&b->cv_consumatore, &b->mutex);
    }

    strncpy(caratteri, b->buffer, b->num_occupati);
    num_liberati = b->num_occupati;
    b->num_occupati = 0;

    pthread_cond_signal(&b->cv_produttore);

    pthread_mutex_unlock(&b->mutex);

    return num_liberati;
}