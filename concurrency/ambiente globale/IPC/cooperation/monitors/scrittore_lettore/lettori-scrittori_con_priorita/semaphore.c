#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "semaphore.h"

void Wait_Sem(int semid, int numsem) {
    struct sembuf sem_buf;

    sem_buf.sem_num = numsem;
    sem_buf.sem_op = -1;  // decrementa di -1 il valore del semaforo
    sem_buf.sem_flg = 0;

    int result = semop(semid, &sem_buf, 1);

    if (result < 0) {
        perror("Errore nell'operazione di semop in Wait_Sem");
        exit(EXIT_FAILURE);
    }
}

void Signal_Sem(int semid, int numsem) {
    struct sembuf sem_buf;

    sem_buf.sem_num = numsem;
    sem_buf.sem_op = 1;  // incrementa di 1 il valore del semaforo
    sem_buf.sem_flg = 0;

    int result = semop(semid, &sem_buf, 1);

    if (result < 0) {
        perror("Errore nell'operazione di semop in Signal_Sem");
        exit(EXIT_FAILURE);
    }
}

// restituisce il numero di processi in attesa sul semaforo
int Queue_Sem(int id_sem, int numsem) {
    return semctl(id_sem, numsem, GETNCNT, NULL);
}