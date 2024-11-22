#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "buffer_circolare.h"

/*
    I processi produttore e consumatore dovranno essere istanziati
    utilizzando **due eseguibili separati**. Un terzo programma **master**
    dovrà istanziare 10 processi produttore e 1 processo consumatore, e
    attenderne la terminazione. Tra la creazione di un processo e la
    successiva si dovrà attendere un tempo casuale tra 1 e 3 secondi. Ogni
    produttore dovrà effettuare una sola produzione. Il processo consumatore
    effettuerà 2 accessi al vettore, attendendo 1 secondo tra i 2 accessi.
*/

#define TOTALE_PRODUTTORI 10
#define TOTALE_CONSUMATORI 1

int main() {
    srand(getpid());

    key_t key_sem = ftok(".", 's');

    int id_sem = semget(key_sem, 2, IPC_CREAT | 0644);

    if (id_sem < 0) {
        perror("[Consumatore] Errore nella semget");
        exit(1);
    }

    semctl(id_sem, SPAZIO_DISP, SETVAL, DIM);
    semctl(id_sem, BUFFER_PIENO, SETVAL, 0);

    key_t key_shm = ftok(".", 'm');

    int id_shm = shmget(key_shm, sizeof(struct BufferCircolare), IPC_CREAT | 0644);

    if (id_shm < 0) {
        perror("[Consumatore] Errore nella shmget");
        exit(1);
    }

    struct BufferCircolare* buf = shmat(id_shm, NULL, 0);

    if (buf == (void*)-1) {
        perror("[Consumatore] Errore nella shmat");
        exit(1);
    }

    pid_t pid;
    for (int i = 0; i < TOTALE_CONSUMATORI; i++) {
        pid = fork();

        if (pid < 0) {
            perror("Errore nella creazione del processo consumatore");
            exit(1);
        } else if (pid == 0) {
            execl("./consumatore", "consumatore", NULL);

            perror("Errore nella execl del consumatore");
            exit(1);
        }

        sleep(rand() % 3 + 1);
    }

    for (int i = 0; i < TOTALE_PRODUTTORI; i++) {
        pid = fork();

        if (pid < 0) {
            perror("Errore nella creazione del processo produttore");
            exit(1);
        } else if (pid == 0) {
            execl("./produttore", "produttore", NULL);

            perror("Errore nella execl del produttore");
            exit(1);
        }
        
        sleep(rand() % 3 + 1);
    }

    for (int i = 0; i < TOTALE_CONSUMATORI + TOTALE_PRODUTTORI; i++) {
        wait(NULL);
    }

    semctl(id_sem, 0, IPC_RMID, NULL);
    shmctl(id_shm, IPC_RMID, NULL);
}