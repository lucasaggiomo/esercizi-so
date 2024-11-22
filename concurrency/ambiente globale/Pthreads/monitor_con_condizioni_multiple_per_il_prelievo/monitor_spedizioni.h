#ifndef MONITOR_H
#define MONITOR_H

#include <pthread.h>

#define DIM 4

struct monitor_spedizioni {
    int ordini[DIM];
    int testa;

    int somma;

    pthread_mutex_t mutex;
    pthread_cond_t cv_produttore;
    pthread_cond_t cv_consumatore;
};

void init_monitor(struct monitor_spedizioni* m);
void destroy_monitor(struct monitor_spedizioni* m);

void inserisci(struct monitor_spedizioni* m, int quantita);
int preleva(struct monitor_spedizioni* m, int quantita_minima);

#endif