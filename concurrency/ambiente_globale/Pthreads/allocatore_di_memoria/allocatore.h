#ifndef ALLOCATORE_H
#define ALLOCATORE_H

#include <pthread.h>

enum stato {
    LIBERO,
    OCCUPATO
};

#define SIZE_MEMORY 20

struct Allocatore {
    char memoria[SIZE_MEMORY];         // array di caratteri da allocare
    enum stato stato[SIZE_MEMORY];     // array di stato dei caratteri da allocare

    pthread_mutex_t mutex;
    pthread_cond_t cv_richiesta;
};

// inizializza la struttura dati
void AllocInit(struct Allocatore* a);

// dealloca i membri dell'allocatore
void AllocDestroy(struct Allocatore* a);

// restituisce l'offset nella memoria del primo blocco libero di almeno n byte, -1 se non presente
int first_fit(struct Allocatore* a, int n);

// alloca n caratteri (first-fit)
char* getMemoria(struct Allocatore* a, int n);

// dealloca n caratteri
void releaseMemoria(struct Allocatore* a, char* blocco, int n);

void printMemoryState(struct Allocatore* a);

#endif