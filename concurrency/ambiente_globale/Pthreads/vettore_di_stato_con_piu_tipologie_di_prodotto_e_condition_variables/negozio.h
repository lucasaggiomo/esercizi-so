#ifndef NEGOZIO_H
#define NEGOZIO_H

#include <pthread.h>

#include "articolo.h"

#define NUM_ARTICOLI 10

struct negozio {
    struct articolo articoli[NUM_ARTICOLI];
    int num_scarpe;
    int num_giacche;
    int num_liberi;

    pthread_mutex_t mutex;
    pthread_cond_t cv_produttore;
    pthread_cond_t cv_consumatore_scarpe;
    pthread_cond_t cv_consumatore_giacche;
};

void init_negozio(struct negozio* n);
void destroy_negozio(struct negozio* n);

void inserisci_scarpe(struct negozio* n);
void inserisci_giacca(struct negozio* n);
void preleva_scarpe(struct negozio* n);
void preleva_giacca(struct negozio* n);

int inizio_produzione(struct negozio* n);
void fine_produzione(struct negozio* n, enum tipo tipo, int index);

int inizio_consumo(struct negozio* n, enum tipo tipo);
void fine_consumo(struct negozio* n, int index);

#endif