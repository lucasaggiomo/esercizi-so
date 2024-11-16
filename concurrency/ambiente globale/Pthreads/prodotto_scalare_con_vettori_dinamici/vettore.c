#include "vettore.h"
#include <stdio.h>
#include <stdlib.h>

void inizializza(struct Vettore* v, int dimensione) {
    if (dimensione < 0) {
        perror("Errore, la dimensione del vettore era negativa!");
        return;
    }

    v->vettore = malloc(dimensione * sizeof(*v->vettore));
    if (!v->vettore) {
        perror("Errore nell'allocazione del vettore");
        return;
    }

    v->dimensione = dimensione;
    v->testa = 0;
    v->coda = 0;
    v->count = 0;

    pthread_mutex_init(&v->mutex, NULL);
    pthread_cond_init(&v->cv_inserimento, NULL);
    pthread_cond_init(&v->cv_prelevazione, NULL);
}

void inserisci_elemento(struct Vettore* v, int elemento) {
    pthread_mutex_lock(&v->mutex);

    // si blocca se il vettore è pieno
    while (v->count == v->dimensione) {
        pthread_cond_wait(&v->cv_inserimento, &v->mutex);
    }

    // inserisce l'elemento
    v->vettore[v->testa] = elemento;
    v->testa = (v->testa + 1) % v->dimensione;
    v->count++;

    printf("Inserimento di %d. Ora count = %d\n", elemento, v->count);

    // segnala l'inserimento svegliando i thread che prelevano (qualora ce ne fossero in attesa)
    pthread_cond_signal(&v->cv_prelevazione);

    pthread_mutex_unlock(&v->mutex);
}

int preleva_elemento(struct Vettore* v) {
    int elemento;

    pthread_mutex_lock(&v->mutex);

    // si blocca se il vettore è vuoto
    while (v->count == 0) {
        pthread_cond_wait(&v->cv_prelevazione, &v->mutex);
    }

    // preleva l'elemento
    elemento = v->vettore[v->coda];
    v->coda = (v->coda + 1) % v->dimensione;
    v->count--;

    printf("Prelevazione di %d. Ora count = %d\n", elemento, v->count);

    // segnala la prelevazione svegliando i thread che inseriscono (qualora ce ne fossero in attesa)
    pthread_cond_signal(&v->cv_inserimento);

    pthread_mutex_unlock(&v->mutex);

    return elemento;
}

void distruggi(struct Vettore* v) {
    free(v->vettore);

    pthread_mutex_destroy(&v->mutex);
    pthread_cond_destroy(&v->cv_inserimento);
    pthread_cond_destroy(&v->cv_prelevazione);
}