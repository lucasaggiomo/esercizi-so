#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "semaphore.h"

#include "buffer.h"

void init_buffer(struct buffer* b) {
    b->numero_messaggi = 0;
    b->totale = 0;
    b->id_sem = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);
    if (b->id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }
    semctl(b->id_sem, MUTEX, SETVAL, 1);
}

void destroy_buffer(struct buffer* b) {
    semctl(b->id_sem, 0, IPC_RMID, 0);
}

void scrivi(struct buffer* b, int quantita) {
    Wait_Sem(b->id_sem, MUTEX);

    b->numero_messaggi++;
    b->totale += quantita;

    Signal_Sem(b->id_sem, MUTEX);
}