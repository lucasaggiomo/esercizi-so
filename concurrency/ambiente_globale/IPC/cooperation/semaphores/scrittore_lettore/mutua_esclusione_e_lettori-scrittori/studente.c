#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "header.h"

int main() {
    key_t key_sem = ftok(".", 's');
    int id_sem = semget(key_sem, 3, 0);
    if (id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }
    key_t key_shm = ftok(".", 'e');
    int id_shm = shmget(key_shm, sizeof(esame_t), 0);
    if (id_shm < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    esame_t* e = shmat(id_shm, NULL, 0);
    if (e == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    srand(getpid());

    int secondi = rand() % 9;
    sleep(secondi);

    inizio_lettura(id_sem, e);
    printf("[Studente %d] La data del prossimo appello è la seguente: %s\n", getpid(), e->prossimo_appello);
    fine_lettura(id_sem, e);

    accedi_prenotati(id_sem);
    e->numero_prenotati++;
    printf("[Studente %d] Ua mi prenoto allo\n", getpid());
    lascia_prenotati(id_sem);

    return 0;
}