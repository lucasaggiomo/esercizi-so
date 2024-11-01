#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../semaphore.h"

#define SPAZIO_DISP 0  // spazio disponibile
#define MSG_DISP 1     // messaggio disponibile

void produttore(int* p_sh, int semid) {
    // decrementa il semaforo SPAZIO_DISP e si blocca se il valore diventa negativo
    Wait_Sem(semid, SPAZIO_DISP);

    *p_sh = rand() % 16;
    printf("Produzione: %d\n", *p_sh);

    // incrementa il semaforo MSG_DISP in modo da svegliare il consumatore in coda
    Signal_Sem(semid, MSG_DISP);
}

void consumatore(int* p_sh, int semid) {
    // decrementa il semaforo MSG_DISP e si blocca se il valore diventa negativo
    Wait_Sem(semid, MSG_DISP);

    printf("Consumatore: %d\n", *p_sh);

    // incrementa il semaforo SPAZIO_DISP in modo da svegliare il produttore in coda
    Signal_Sem(semid, SPAZIO_DISP);
}

int main() {
    srand(time(NULL));

    // creo due semafori (array di 2 semafori)
    int semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0644);

    // gestisco eventuali errori
    if (semid < 0) {
        perror("Errore nella creazione dei semafori");
        exit(EXIT_FAILURE);
    }

    // inizializzo i semafori
    int result = semctl(semid, SPAZIO_DISP, SETVAL, 1);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo SPAZIO_DISP");
        exit(EXIT_FAILURE);
    }

    result = semctl(semid, MSG_DISP, SETVAL, 0);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo MSG_DISP");
        exit(EXIT_FAILURE);
    }

    // creo la memoria condivisa

    int ds_shm = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0644);

    if (ds_shm < 0) {
        perror("Errore nella creazione della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // collego ("attach") il segmento di memoria allo spazio di indirizzamento del chiamante
    int* p = shmat(ds_shm, NULL, 0);

    if (p == (void*)-1) {
        perror("Errore nel colllegamento della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // creo il produttore (che scrive su p)
    int pid = fork();

    // sia il processo padre sia il figlio ottengono una copia
    // del puntatore a memoria condivisa "p"

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del produttore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        for (int i = 0; i < 4; i++) {
            produttore(p, semid);
        }

        // termina l'esecuzione
        exit(EXIT_SUCCESS);
    }

    // creo il
    pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del consumatore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        for (int i = 0; i < 4; i++) {
            consumatore(p, semid);
        }

        // termina l'esecuzione
        exit(EXIT_SUCCESS);
    }

    // il padre attende la terminazione dei due figli
    wait(NULL);
    wait(NULL);

    // elimino la risorsa dell'array di semafori
    result = semctl(semid, 0, IPC_RMID);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nella cancellazione dei semafori");
        exit(EXIT_FAILURE);
    }

    // elimino la risorsa condivisa
    result = shmctl(ds_shm, IPC_RMID, NULL);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nella cancellazione della risorsa condivisa");
        exit(EXIT_FAILURE);
    }

    return 0;
}