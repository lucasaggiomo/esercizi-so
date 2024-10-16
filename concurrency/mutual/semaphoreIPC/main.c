#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../utils/semaphore.h"

// void enter_region(int tid) {
//     wait_semaphore(&s, tid);
// }
// void leave_region(int tid) {
//     signal_semaphore(&s);
// }

#define ITERATIONS 100000

void enter_region(int semid, int numsem) {
    Wait_Sem(semid, numsem);
}

void leave_region(int semid, int numsem) {
    Signal_Sem(semid, numsem);
}

void critical_region(int* counter) {
    (*counter)++;
}

void do_stuff(int semid, int numsem, int* counter, char process_id) {
    printf("%c: begin\n", process_id);

    for (int i = 0; i < ITERATIONS; i++) {
        enter_region(semid, numsem);

        critical_region(counter);

        leave_region(semid, numsem);
    }

    printf("%c: done\n", process_id);
}

int main() {
    // creo un semaforo
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);

    // gestisco eventuali errori
    if (semid < 0) {
        perror("Errore nella creazione del semaforo");
        exit(EXIT_FAILURE);
    }

    // inizializzo il semaforo
    int result = semctl(semid, 0, SETVAL, 1);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo");
        exit(EXIT_FAILURE);
    }

    // creo la memoria condivisa

    int counter_shm = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0644);

    if (counter_shm < 0) {
        perror("Errore nella creazione della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // collego ("attach") il segmento di memoria allo spazio di indirizzamento del chiamante
    int* counter = shmat(counter_shm, NULL, 0);

    if (counter == (void*)-1) {
        perror("Errore nel colllegamento della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // inizializzo la risorsa condivisa
    *counter = 0;

    // creo due processi figli che incrementeranno counter
    int pid = fork();

    // sia il processo padre sia il figlio ottengono una copia
    // del puntatore a memoria condivisa "p"

    if (pid < 0) {
        // errore
        perror("Errore nella prima fork");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        do_stuff(semid, 0, counter, 'A');

        // termina l'esecuzione
        exit(EXIT_SUCCESS);
    }

    pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella seconda fork");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        do_stuff(semid, 0, counter, 'B');

        // termina l'esecuzione
        exit(EXIT_SUCCESS);
    }

    // il padre attende la terminazione dei due figli
    wait(NULL);
    wait(NULL);

    printf("Counter al termine: %d\n", *counter);

    // elimino la risorsa del semaforo
    result = semctl(semid, 0, IPC_RMID);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nella cancellazione del semaforo");
        exit(EXIT_FAILURE);
    }

    // elimino la risorsa condivisa
    result = shmctl(counter_shm, IPC_RMID, NULL);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nella cancellazione della risorsa condivisa");
        exit(EXIT_FAILURE);
    }

    return 0;
}