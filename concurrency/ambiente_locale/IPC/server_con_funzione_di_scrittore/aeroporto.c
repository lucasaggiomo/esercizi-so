#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>

#include "semaphore.h"

#include "aeroporto.h"

void init_aeroporto(struct aeroporto* a) {
    a->num_lettori = 0;

    a->id_sem = semget(IPC_PRIVATE, 2, IPC_CREAT | 0644);
    if (a->id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }

    semctl(a->id_sem, SYNCH, SETVAL, 1);
    semctl(a->id_sem, MUTEX_L, SETVAL, 1);
}

void destroy_aeroporto(struct aeroporto* a) {
    semctl(a->id_sem, 0, IPC_RMID, 0);
}

// implementazione del paradigma LETTORI - SCRITTORI con starvation di entrambi, ma con un solo scrittore
// quindi non mi serve il semaforo MUTEX_S, né MUTEX_RISORSA, né num_scrittori => si riduce all'algoritmo con starvation dei soli scrittori
void scrivi_gate(struct aeroporto* a, int num_gate, struct gate* gate) {
    Wait_Sem(a->id_sem, SYNCH);

    memcpy(&a->gates[num_gate], gate, sizeof(struct gate));

    Signal_Sem(a->id_sem, SYNCH);
}

void leggi_aeroporto(struct aeroporto* a, struct gate gates[NUM_GATE]) {
    Wait_Sem(a->id_sem, MUTEX_L);

    a->num_lettori++;
    if (a->num_lettori == 1) {
        Wait_Sem(a->id_sem, SYNCH);
    }

    Signal_Sem(a->id_sem, MUTEX_L);

    memcpy(gates, a->gates, NUM_GATE * sizeof(struct gate));

    Wait_Sem(a->id_sem, MUTEX_L);

    a->num_lettori--;
    if (a->num_lettori == 0) {
        Signal_Sem(a->id_sem, SYNCH);
    }

    Signal_Sem(a->id_sem, MUTEX_L);
}
