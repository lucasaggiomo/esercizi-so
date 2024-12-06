#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "negozio.h"

#define NUM_PRODUZIONI 10

void* produttore(void* arg);
void* consumatore_scarpe(void* arg);
void* consumatore_giacca(void* arg);

int main() {
    int ret;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_produttore;
    pthread_t id_consumatore_scarpe;
    pthread_t id_consumatore_giacca;

    struct negozio* n = malloc(sizeof(*n));
    if (!n) {
        perror("Errore nella malloc");
        exit(1);
    }
    init_negozio(n);

    ret = pthread_create(&id_produttore, &attr, produttore, n);
    if (ret < 0) {
        perror("Errore nella creazione del produttore");
        exit(1);
    }

    ret = pthread_create(&id_consumatore_scarpe, &attr, consumatore_scarpe, n);
    if (ret < 0) {
        perror("Errore nella creazione del consumatore di scarpe");
        exit(1);
    }

    ret = pthread_create(&id_consumatore_giacca, &attr, consumatore_giacca, n);
    if (ret < 0) {
        perror("Errore nella creazione del consumatore di giacche");
        exit(1);
    }

    pthread_join(id_produttore, NULL);
    pthread_join(id_consumatore_scarpe, NULL);
    pthread_join(id_consumatore_giacca, NULL);

    destroy_negozio(n);
    free(n);

    pthread_attr_destroy(&attr);

    return 0;
}

void* produttore(void* arg) {
    struct negozio* n = arg;
    for (int i = 0; i < NUM_PRODUZIONI; i++) {
        inserisci_scarpe(n);
        inserisci_giacca(n);
    }
}
void* consumatore_scarpe(void* arg) {
    struct negozio* n = arg;
    for (int i = 0; i < NUM_PRODUZIONI; i++) {
        preleva_scarpe(n);
    }
}
void* consumatore_giacca(void* arg) {
    struct negozio* n = arg;
    for (int i = 0; i < NUM_PRODUZIONI; i++) {
        preleva_giacca(n);
    }
}