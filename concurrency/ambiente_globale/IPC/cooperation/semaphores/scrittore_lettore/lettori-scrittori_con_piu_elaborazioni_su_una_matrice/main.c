#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "matrice.h"

int main() {
    key_t key_sem = ftok(".", 's');
    int id_sem = semget(key_sem, 4, IPC_CREAT | 0644);
    if (id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }

    semctl(id_sem, SYNCH, SETVAL, 1);
    semctl(id_sem, MUTEX_L, SETVAL, 1);
    semctl(id_sem, MUTEX_S, SETVAL, 1);
    semctl(id_sem, MUTEX_RISORSA, SETVAL, 1);

    key_t key_shm = ftok(".", 'm');
    int id_shm = shmget(key_shm, sizeof(struct matrice), IPC_CREAT | 0644);
    if (id_shm < 0) {
        perror("Errore nella shmget della matrice");
        exit(1);
    }

    struct matrice* m = shmat(id_shm, NULL, 0);
    if (m == (void*)-1) {
        perror("Errore nella shmat della matrice");
        exit(1);
    }

    // inizializzo la matrice
    m->side = rand() % 3 + 3;
    m->num_lettori = 0;
    m->num_scrittori = 0;

    key_t key_stop = ftok(".", 'S');
    int id_stop = shmget(key_stop, sizeof(int), IPC_CREAT | 0644);
    if (id_stop < 0) {
        perror("Errore nella shmget dello stop");
        exit(1);
    }

    int* stop = shmat(id_stop, NULL, 0);
    if (stop == (void*)-1) {
        perror("Errore nella shmat dello stop");
        exit(1);
    }

    *stop = 0;

    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./generatore", "generatore", NULL);
        perror("Errore nella execl del generatore");
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./generatore", "generatore", NULL);
        perror("Errore nella execl del generatore");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./elaboratore", "elaboratore", NULL);
        perror("Errore nella execl dell'elaboratore");
        exit(1);
    }

    for (int i = 0; i < 2; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            execl("./analizzatore", "analizzatore", NULL);
            perror("Errore nella execl dell'analizzatore");
            exit(1);
        }
    }

    sleep(15);
    *stop = 1;

    for (int i = 0; i < 4; i++) {
        wait(NULL);
    }

    semctl(id_sem, 0, IPC_RMID, NULL);
    shmctl(id_shm, IPC_RMID, NULL);
    shmctl(id_stop, IPC_RMID, NULL);

    return 0;
}