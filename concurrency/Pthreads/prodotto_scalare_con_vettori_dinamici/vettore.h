#ifndef VETTORE_H
#define VETTORE_H

#include <pthread.h>

struct Vettore {
    int* vettore;  // puntatore ad un vettore di interi, da allocare su heap
    int dimensione;
    int testa;
    int coda;
    int count;

    pthread_mutex_t mutex;
    pthread_cond_t cv_inserimento;
    pthread_cond_t cv_prelevazione;
};

void inizializza(struct Vettore* v, int dimensione);
void inserisci_elemento(struct Vettore* v, int elemento);
int preleva_elemento(struct Vettore* v);
void distruggi(struct Vettore* v);

#endif