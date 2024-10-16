#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../../utils/semaphore.h"

#define SPAZIO_DISP 0  // spazio disponibile
#define MSG_DISP 1     // messaggio disponibile

#define DIM 3

// struct per la coda circolare
struct queue {
    int buffer[DIM];
    int head;
    int tail;
};

void enqueue(struct queue* q, int value) {
    q->buffer[q->head] = value;
    q->head = (q->head + 1) % DIM;
}
int dequeue(struct queue* q) {
    int output = q->buffer[q->tail];
    q->tail = (q->tail + 1) % DIM;
    return output;
}

void produttore(struct queue* q_sh, int semid) {
    // decrementa il semaforo SPAZIO_DISP e si blocca se il valore diventa negativo
    Wait_Sem(semid, SPAZIO_DISP);

    int value = rand() % 16;
    enqueue(q_sh, value);
    printf("Produzione: %d\n", value);

    // incrementa il semaforo MSG_DISP in modo da svegliare il consumatore in coda
    Signal_Sem(semid, MSG_DISP);
}

void consumatore(struct queue* q_sh, int semid) {
    // decrementa il semaforo MSG_DISP e si blocca se il valore diventa negativo
    Wait_Sem(semid, MSG_DISP);

    int value = dequeue(q_sh);
    printf("Consumatore: %d\n", value);

    sleep(1);

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
    int result = semctl(semid, SPAZIO_DISP, SETVAL, DIM);  // inizialmente è possibile produrre DIM volte

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo SPAZIO_DISP");
        exit(EXIT_FAILURE);
    }

    result = semctl(semid, MSG_DISP, SETVAL, 0);  // inizialmente non è possibile consumare nulla

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo MSG_DISP");
        exit(EXIT_FAILURE);
    }

    // creo la memoria condivisa

    int ds_shm = shmget(IPC_PRIVATE, sizeof(struct queue), IPC_CREAT | 0644);

    if (ds_shm < 0) {
        perror("Errore nella creazione della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // collego ("attach") il segmento di memoria allo spazio di indirizzamento del chiamante
    struct queue* q = shmat(ds_shm, NULL, 0);

    if (q == (void*)-1) {
        perror("Errore nel colllegamento della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // inizializzo head e tail
    q->head = 0;
    q->tail = 0;

    // creo il produttore (che scrive su q)
    int pid = fork();

    // sia il processo padre sia il figlio ottengono una copia
    // del puntatore a memoria condivisa "q"

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del produttore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        for (int i = 0; i < 4; i++) {
            produttore(q, semid);
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
            consumatore(q, semid);
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