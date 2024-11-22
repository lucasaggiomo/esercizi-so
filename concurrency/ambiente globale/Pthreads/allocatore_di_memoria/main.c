/*
Si sviluppi inoltre un programma che utilizzi l'allocatore. Esso deve
istanziare 5 thread, ciascuno dei quali alloca un blocco di dimensione
casuale (da 1 a 3 caratteri), attende 3 secondi e rilascia il blocco.
Una volta generati i thread (come joinable), il programma principale ne
attende la terminazione, e termina a sua volta.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "allocatore.h"

#define _GNU_SOURCE

#define TOTALE_THREADS 10

void* thread_method(void* arg) {
    struct Allocatore* a = arg;

    srand(gettid());

    int size = rand() % 10 + 3;

    char* blocco = getMemoria(a, size);

    sleep(rand() % 3 + 2);

    releaseMemoria(a, blocco, size);

    pthread_exit(NULL);
}

int main() {
    srand(getpid());

    pthread_t id_threads[TOTALE_THREADS];

    // alloco un Allocatore con malloc
    struct Allocatore* allocatore = malloc(sizeof(struct Allocatore));
    if (!allocatore) {
        perror("Errore nell'allocazione dell'allocatore");
        exit(1);
    }

    AllocInit(allocatore);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < TOTALE_THREADS; i++) {
        int ret = pthread_create(&id_threads[i], &attr, thread_method, allocatore);
        if (ret < 0) {
            perror("Errore nella creazione del thread");
            exit(1);
        }
        sleep(rand() % 3);
    }

    for (int i = 0; i < TOTALE_THREADS; i++) {
        int ret = pthread_join(id_threads[i], NULL);
        if (ret < 0) {
            perror("Errore nella join del thread");
            exit(1);
        }
    }

    pthread_attr_destroy(&attr);

    AllocDestroy(allocatore);

    free(allocatore);
}