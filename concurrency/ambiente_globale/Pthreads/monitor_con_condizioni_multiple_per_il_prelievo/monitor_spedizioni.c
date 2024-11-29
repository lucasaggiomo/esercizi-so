#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#define _GNU_SOURCE

#include "monitor_spedizioni.h"

void init_monitor(struct monitor_spedizioni* m) {
    pthread_mutex_init(&m->mutex, NULL);
    pthread_cond_init(&m->cv_produttore, NULL);
    pthread_cond_init(&m->cv_consumatore, NULL);

    m->somma = 0;
    m->testa = 0;
}

void destroy_monitor(struct monitor_spedizioni* m) {
    pthread_mutex_destroy(&m->mutex);
    pthread_cond_destroy(&m->cv_produttore);
    pthread_cond_destroy(&m->cv_consumatore);
}

void inserisci(struct monitor_spedizioni* m, int quantita) {
    pthread_mutex_lock(&m->mutex);

    while (m->testa == DIM) {
        pthread_cond_wait(&m->cv_produttore, &m->mutex);
    }

    printf("[Produttore %d] Inserisco %d\n", gettid(), quantita);

    m->ordini[m->testa] = quantita;
    m->testa = (m->testa + 1) % DIM;

    m->somma += quantita;

    printf("\n[");
    for (int i = 0; i < m->testa; i++) {
        printf(" %d", m->ordini[i]);
        if (i != m->testa - 1)
            printf(" -");
    }
    printf(" ]\nSomma %d\n\n", m->somma);

    pthread_cond_broadcast(&m->cv_consumatore);

    pthread_mutex_unlock(&m->mutex);
}

int preleva(struct monitor_spedizioni* m, int quantita_minima) {
    pthread_mutex_lock(&m->mutex);

    while (m->testa < DIM && m->somma < quantita_minima) {
        pthread_cond_wait(&m->cv_consumatore, &m->mutex);
    }

    printf("[Consumatore %d] Prelevo tutto\n", gettid());

    // """""preleva""""" tutti gli ordini (cioè pone testa (e somma) a 0)
    m->testa = 0;
    int somma = m->somma;
    m->somma = 0;

    pthread_cond_broadcast(&m->cv_produttore);

    pthread_mutex_unlock(&m->mutex);

    return somma;
}