#include <stdio.h>
#include <stdlib.h>

#include "dvd.h"

void init_monitor(struct Monitor* m) {
    pthread_mutex_init(&m->mutex, NULL);

    for (int i = 0; i < NUM_IDENTIFICATIVI; i++) {
        pthread_cond_init(&m->cv_affittuari[i], NULL);

        m->num_disponibili[i] = NUM_COPIE;

        for (int k = 0; k < NUM_COPIE; k++) {
            m->dvd[i * NUM_COPIE + k].identificativo_film = i + 1;
            m->dvd[i * NUM_COPIE + k].identificativo_copia = k + 1;
            m->dvd[i * NUM_COPIE + k].stato = DISPONIBILE;
        }
    }
}

void destroy_monitor(struct Monitor* m) {
    pthread_mutex_destroy(&m->mutex);

    for (int i = 0; i < NUM_IDENTIFICATIVI; i++) {
        pthread_cond_destroy(&m->cv_affittuari[i]);
    }
}

int affitta(struct Monitor* m, int id_film) {
    int index = 0;
    int id_copia;

    pthread_mutex_lock(&m->mutex);

    while (m->num_disponibili[id_film - 1] == 0) {
        pthread_cond_wait(&m->cv_affittuari[id_film - 1], &m->mutex);
    }

    while (!(m->dvd[index].identificativo_film == id_film && m->dvd[index].stato == DISPONIBILE)
           && index < NUM_DVDs) {
        index++;
    }

    if (index == NUM_DVDs) {
        perror("Errore: dvd disponibile non trovato");
        exit(1);
    }

    id_copia = m->dvd[index].identificativo_copia;

    m->num_disponibili[id_film - 1]--;
    m->dvd[index].stato = AFFITTATO;

    pthread_mutex_unlock(&m->mutex);

    return id_copia;
}

void restituisci(struct Monitor* m, int id_film, int id_copia) {
    pthread_mutex_lock(&m->mutex);

    int index = 0;
    while (m->dvd[index].identificativo_film != id_film || m->dvd[index].identificativo_copia != id_copia && index < NUM_DVDs) {
        index++;
    }

    if (index == NUM_DVDs) {
        perror("Errore: dvd non trovato");
        exit(1);
    }

    m->dvd[index].stato = DISPONIBILE;
    m->num_disponibili[id_film - 1]++;

    pthread_cond_signal(&m->cv_affittuari[id_film - 1]);

    pthread_mutex_unlock(&m->mutex);
}

void stampa(struct Monitor* m) {
    pthread_mutex_lock(&m->mutex);

    printf("\n-------------------------------------------\n");
    for (int i = 0; i < NUM_DVDs; i++) {
        printf("ID_FILM: %d\tID_COPIA: %d\t%s\n",
               m->dvd[i].identificativo_film,
               m->dvd[i].identificativo_copia,
               stato_to_string(m->dvd[i].stato));

        if (m->dvd[i].identificativo_copia == 2 && i != NUM_DVDs - 1) {
            printf("\n");
        }
    }
    printf("-------------------------------------------\n\n");

    pthread_mutex_unlock(&m->mutex);
}

const char* stato_to_string(enum stato s) {
    switch (s) {
        case DISPONIBILE:
            return "DISPONIBILE";
        case AFFITTATO:
            return "AFFITTATO";
        default:
            return "UNKNOWN";
    }
}