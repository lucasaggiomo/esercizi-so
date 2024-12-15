#ifndef AEROPORTO_H
#define AEROPORTO_H

enum stato {
    PARTENZA,
    ARRIVO
};

struct gate {
    int codice;
    enum stato stato;     // PARTENZA = 0, ARRIVO = 1
};

#define NUM_GATE 4

struct aeroporto {
    struct gate gates[NUM_GATE];

    int id_sem;
    int num_lettori;
};

#define SYNCH 0
#define MUTEX_L 1

void init_aeroporto(struct aeroporto* a);
void destroy_aeroporto(struct aeroporto* a);

void scrivi_gate(struct aeroporto* a, int num_gate, struct gate* gate);
void leggi_aeroporto(struct aeroporto* a, struct gate gates[NUM_GATE]);

#endif