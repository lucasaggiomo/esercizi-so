#ifndef BUFCHAR_H
#define BUFCHAR_H

#include <pthread.h>

#define DIM_BUFFER 10

struct BufChar {
    char buffer[DIM_BUFFER];
    int num_occupati;

    pthread_mutex_t mutex;
    pthread_cond_t cv_produttore;
    pthread_cond_t cv_consumatore;
};

void inizializza(struct BufChar* b);
void distruggi(struct BufChar* b);

void produci(struct BufChar* b, const char* caratteri, int num_char);

// ritorna numero_caratteri consumati
int consuma(struct BufChar* b, char* caratteri);

#endif