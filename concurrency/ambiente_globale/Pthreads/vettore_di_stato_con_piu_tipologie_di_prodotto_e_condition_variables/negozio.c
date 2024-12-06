#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>

#include "negozio.h"

extern pid_t gettid();

void init_negozio(struct negozio* n) {
    n->num_giacche = 0;
    n->num_scarpe = 0;
    n->num_liberi = NUM_ARTICOLI;

    for (int i = 0; i < NUM_ARTICOLI; i++) {
        n->articoli->stato = LIBERO;
    }

    pthread_mutex_init(&n->mutex, NULL);
    pthread_cond_init(&n->cv_produttore, NULL);
    pthread_cond_init(&n->cv_consumatore_giacche, NULL);
    pthread_cond_init(&n->cv_consumatore_scarpe, NULL);
}
void destroy_negozio(struct negozio* n) {
    pthread_mutex_destroy(&n->mutex);
    pthread_cond_destroy(&n->cv_produttore);
    pthread_cond_destroy(&n->cv_consumatore_giacche);
    pthread_cond_destroy(&n->cv_consumatore_scarpe);
}

void inserisci_scarpe(struct negozio* n) {
    int i = inizio_produzione(n);

    n->articoli[i].tipo = SCARPE;
    printf("[Calzolaio %d] Ho prodotto un paio di scarpe nella posizione %d\n", gettid(), i);

    fine_produzione(n, SCARPE, i);
}

void inserisci_giacca(struct negozio* n) {
    int i = inizio_produzione(n);

    n->articoli[i].tipo = GIACCA;
    printf("[Sarto %d] Ho prodotto una giaccia nella posizione %d\n", gettid(), i);

    fine_produzione(n, GIACCA, i);
}

void preleva_scarpe(struct negozio* n) {
    int i = inizio_consumo(n, SCARPE);

    sleep(1);
    printf("[Consumatore di scarpe %d] Ho consumato un paio di scarpe nella posizione %d\n", gettid(), i);

    fine_consumo(n, i);
}

void preleva_giacca(struct negozio* n) {
    int i = inizio_consumo(n, GIACCA);

    sleep(1);
    printf("[Consumatore di giacche %d] Ho consumato una giacca nella posizione %d\n", gettid(), i);

    fine_consumo(n, i);
}

// FUNZIONI DI SINCRONIZZAZIONE
int inizio_produzione(struct negozio* n) {
    int index = 0;

    pthread_mutex_lock(&n->mutex);

    while (n->num_liberi == 0) {
        pthread_cond_wait(&n->cv_produttore, &n->mutex);
    }

    while (n->articoli[index].stato != LIBERO && index < NUM_ARTICOLI) {
        index++;
    }

    n->articoli[index].stato = IN_USO;
    n->num_liberi--;

    pthread_mutex_unlock(&n->mutex);

    return index;
}

void fine_produzione(struct negozio* n, enum tipo tipo, int index) {
    pthread_mutex_lock(&n->mutex);

    n->articoli[index].stato = OCCUPATO;
    switch (tipo) {
        case GIACCA:
            n->num_giacche++;
            pthread_cond_signal(&n->cv_consumatore_giacche);
            break;
        case SCARPE:
            n->num_scarpe++;
            pthread_cond_signal(&n->cv_consumatore_scarpe);
            break;
        default:
            fprintf(stderr, "WTF\n");
            pthread_mutex_unlock(&n->mutex);
            exit(1);
            break;
    }

    pthread_mutex_unlock(&n->mutex);
}

int inizio_consumo(struct negozio* n, enum tipo tipo) {
    int index = 0;

    pthread_mutex_lock(&n->mutex);

    // switch perché mi scoccio di fare due funzioni per tipo distinto
    int* num_tipo;
    pthread_cond_t* cv_consumatore;
    switch (tipo) {
        case SCARPE:
            num_tipo = &n->num_scarpe;
            cv_consumatore = &n->cv_consumatore_scarpe;
            break;
        case GIACCA:
            num_tipo = &(n->num_giacche);
            cv_consumatore = &n->cv_consumatore_giacche;
            break;
        default:
            fprintf(stderr, "WTF\n");
            pthread_mutex_unlock(&n->mutex);
            exit(1);
            break;
    }

    while (*num_tipo == 0) {
        pthread_cond_wait(cv_consumatore, &n->mutex);
    }

    while (!(n->articoli[index].stato == OCCUPATO && n->articoli[index].tipo == tipo)
           && index < NUM_ARTICOLI) {
        index++;
    }

    n->articoli[index].stato = IN_USO;
    (*num_tipo)--;

    pthread_mutex_unlock(&n->mutex);

    return index;
}

void fine_consumo(struct negozio* n, int index) {
    pthread_mutex_lock(&n->mutex);

    n->articoli[index].stato = LIBERO;
    n->num_liberi++;
    pthread_cond_signal(&n->cv_produttore);

    pthread_mutex_unlock(&n->mutex);
}