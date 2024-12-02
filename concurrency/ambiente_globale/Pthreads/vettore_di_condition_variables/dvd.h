#ifndef DVD_H
#define DVD_H

#include <pthread.h>

enum stato {
    DISPONIBILE,
    AFFITTATO
};

const char* stato_to_string(enum stato s);

#define NUM_DVDs 6
#define NUM_IDENTIFICATIVI 3
#define NUM_COPIE 2

struct DVD {
    int identificativo_film;      // un intero tra 1 e 3
    int identificativo_copia;     // un intero tra 1 e 2
    enum stato stato;             // DISPONIBILE=0, oppure AFFITTATO=1
};

struct Monitor {
    struct DVD dvd[NUM_DVDs];                    // vettore di DVD da gestire
    int num_disponibili[NUM_IDENTIFICATIVI];     // num disponibili per identificativo film

    pthread_mutex_t mutex;
    pthread_cond_t cv_affittuari[NUM_IDENTIFICATIVI];
};

void init_monitor(struct Monitor* m);
void destroy_monitor(struct Monitor* m);

int affitta(struct Monitor* m, int id_film);
void restituisci(struct Monitor* m, int id_film, int id_copia);
void stampa(struct Monitor* m);

#endif