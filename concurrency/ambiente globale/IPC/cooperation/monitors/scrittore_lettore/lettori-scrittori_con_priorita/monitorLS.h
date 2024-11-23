#ifndef MONITORLS_H
#define MONITORLS_H

#include "monitor_hoare.h"

struct MonitorLS {
    int buffer;

    struct Monitor monitor;
    int num_lettori;
    int num_scrittori_alta_prio;
    int num_scrittori_bassa_prio;
};

#define CV_SCRITTORI_ALTA_PRIO 0
#define CV_SCRITTORI_BASSA_PRIO 1
#define CV_LETTORI 2

void inizializza(struct MonitorLS* m);
void distruggi(struct MonitorLS* m);

int leggi(struct MonitorLS* m);
void scrivi_lento(struct MonitorLS* m, int valore);
void scrivi_veloce(struct MonitorLS* m, int valore);

#endif