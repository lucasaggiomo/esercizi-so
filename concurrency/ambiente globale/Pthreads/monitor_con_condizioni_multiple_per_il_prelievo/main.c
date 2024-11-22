#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "monitor_spedizioni.h"

/*
    Il programma principale dovrà creare 3 thread, ciascuno dei quali effettuerà
    5 inserimenti in un ciclo (con una attesa di 1 secondo ad ogni iterazione del ciclo).
    Al primo inserimento, il thread inserisce il valore 1; al secondo, il valore 2;
    al terzo, il valore 3; etc. Inoltre, il programma principale dovrà creare
    un ulteriore thread che chiamerà ripetutamente il metodo preleva, fin quando
    la somma totale di tutti i valori consumati non raggiunga 45.
    Si utilizzi sempre il valore 4 per il parametro quantita_minima.
*/

#define TOTALE_PRODUTTORI 3
#define TOTALE_CONSUMATORI 1

#define NUMERO_INSERIMENTI 5
#define MIN_VALUE 4

#define DESIRED_SOMMA 45

void* produttore(void* arg) {
    struct monitor_spedizioni* m = arg;

    for (int i = 1; i <= NUMERO_INSERIMENTI; i++) {
        inserisci(m, i);
        sleep(1);
    }

    pthread_exit(NULL);
}

void* consumatore(void* arg) {
    struct monitor_spedizioni* m = arg;

    int somma = 0;
    while (somma < DESIRED_SOMMA) {
        somma += preleva(m, MIN_VALUE);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t id_produttori[TOTALE_PRODUTTORI];
    pthread_t id_consumatori[TOTALE_CONSUMATORI];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct monitor_spedizioni* m = malloc(sizeof(struct monitor_spedizioni));
    if (!m) {
        perror("Errore nella malloc");
        exit(1);
    }

    for (int i = 0; i < TOTALE_PRODUTTORI; i++) {
        int ret = pthread_create(&id_produttori[i], &attr, produttore, m);
        if (ret < 0) {
            perror("Errore nella pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < TOTALE_CONSUMATORI; i++) {
        int ret = pthread_create(&id_consumatori[i], &attr, consumatore, m);
        if (ret < 0) {
            perror("Errore nella pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < TOTALE_PRODUTTORI; i++) {
        pthread_join(id_produttori[i], NULL);
    }
    for (int i = 0; i < TOTALE_CONSUMATORI; i++) {
        pthread_join(id_consumatori[i], NULL);
    }

    free(m);

    pthread_attr_destroy(&attr);

    return 0;
}