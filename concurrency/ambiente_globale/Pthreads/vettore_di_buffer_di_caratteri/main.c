#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bufchar.h"

/*
Nel programma principale si creino due thread (rispettivamente
produttore e consumatore). Il produttore dovrà chiamare (per 3 volte,
con periodicità di un secondo) il metodo produci, passando
rispettivamente vettori di 3, 4, e 5 caratteri (scelti a piacere, anche
non random). Il consumatore dovrà chiamare (per 2 volte, con periodicità
di 4 secondi) il metodo consuma, passando in ingresso un vettore vuoto
di 10 caratteri, e stampare a video<sup>[\[3\]](#footnote3)</sup> i caratteri che il metodo
consuma inserisce nel vettore passato in ingresso.
*/

#define NUM_PRODUZIONI 3
#define NUM_CONSUMI 2

#define PRODUZIONI ((const char*[]) { "aaa", "bbbb", "ccccc" })

#define STRLEN_PRODUZIONI \
    (const int[]) { 3, 4, 5 }

void* produttore(void* arg);
void* consumatore(void* arg);

int main() {
    pthread_t id_produttore;
    pthread_t id_consumatore;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct BufChar* b = malloc(sizeof(*b));
    if (!b) {
        perror("Errore nella malloc");
        exit(1);
    }
    inizializza(b);

    int ret;

    ret = pthread_create(&id_produttore, &attr, produttore, b);
    if (ret != 0) {
        perror("Errore nella pthread_create");
        exit(1);
    }

    ret = pthread_create(&id_consumatore, &attr, consumatore, b);
    if (ret != 0) {
        perror("Errore nella pthread_create");
        exit(1);
    }

    printf("Attesa threads...\n");

    ret = pthread_join(id_produttore, NULL);
    if (ret != 0) {
        perror("Errore nella pthread_join");
        exit(1);
    }

    ret = pthread_join(id_consumatore, NULL);
    if (ret != 0) {
        perror("Errore nella pthread_join");
        exit(1);
    }

    printf("Sono tutti morti, distruggo tutto\n");

    distruggi(b);

    free(b);

    pthread_attr_destroy(&attr);

    return 0;
}

void* produttore(void* arg) {
    struct BufChar* b = arg;

    for (int i = 0; i < NUM_PRODUZIONI; i++) {
        printf("[Produttore] Produco %.*s\n", STRLEN_PRODUZIONI[i], PRODUZIONI[i]);

        produci(b, PRODUZIONI[i], STRLEN_PRODUZIONI[i]);

        sleep(1);
    }

    pthread_exit(NULL);
}

void* consumatore(void* arg) {
    struct BufChar* b = arg;
    char buffer[DIM_BUFFER];

    for (int i = 0; i < NUM_CONSUMI; i++) {
        sleep(4);

        int len = consuma(b, buffer);

        printf("[Consumo] Ho consumato %.*s\n", len, buffer);
    }

    pthread_exit(NULL);
}