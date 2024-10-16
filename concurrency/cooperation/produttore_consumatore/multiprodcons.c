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

// definizione indici dei semafori nell'array dei semafori condiviso
#define SPAZIO_DISP 0  // spazio disponibile
#define MSG_DISP 1     // messaggio disponibile
#define MUTEX_P 2      // gestisce la mutua esclusione dei produttori
#define MUTEX_C 3      // gestisce la mutua esclusione dei consumatori

#define DIM 3

#define NUM_PRODUTTORI 2
#define NUM_CONSUMATORI 1

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

    // mutua esclusione tra i produttori per gestire il caso di più produttori (decrementa il semaforo MUTEX_P, se è < 0 si blocca)
    Wait_Sem(semid, MUTEX_P);

    int value = rand() % 16;
    printf("Produzione:\t%d\t(testa = %d)\n", value, q_sh->head);
    sleep(1);

    enqueue(q_sh, value);

    // termina la mutua esclusione (incrementa il semaforo MUTEX_P, se è <= 0 allora sveglia un altro produttore in attesa)
    Signal_Sem(semid, MUTEX_P);

    // incrementa il semaforo MSG_DISP in modo da svegliare il consumatore in coda
    Signal_Sem(semid, MSG_DISP);
}

void consumatore(struct queue* q_sh, int semid) {
    // decrementa il semaforo MSG_DISP e si blocca se il valore diventa negativo
    Wait_Sem(semid, MSG_DISP);

    // Mutua esclusione per il consumatore (analogo al produttore)
    Wait_Sem(semid, MUTEX_C);

    int tail = q_sh->tail;
    int value = dequeue(q_sh);
    printf("Consumatore:\t%d\t(coda = %d)\n", value, tail);

    Signal_Sem(semid, MUTEX_C);

    // incrementa il semaforo SPAZIO_DISP in modo da svegliare il produttore in coda
    Signal_Sem(semid, SPAZIO_DISP);
}

void fork_produttore(struct queue* q_sh, int semid, int produzioni) {
    int pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del produttore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        srand(getpid());
        // processo figlio

        for (int i = 0; i < produzioni; i++) {
            produttore(q_sh, semid);
        }

        // termina l'esecuzione del produttore
        exit(EXIT_SUCCESS);
    }
}

void fork_consumatore(struct queue* q_sh, int semid, int consumazioni) {
    int pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del consumatore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        for (int i = 0; i < consumazioni; i++) {
            consumatore(q_sh, semid);
        }

        // termina l'esecuzione del consumatore
        exit(EXIT_SUCCESS);
    }
}

int main() {
    // creo due semafori (array di 4 semafori)
    int semid = semget(IPC_PRIVATE, 4, IPC_CREAT | 0644);

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

    result = semctl(semid, MUTEX_P, SETVAL, 1);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo MUTEX_P");
        exit(EXIT_FAILURE);
    }

    result = semctl(semid, MUTEX_C, SETVAL, 1);  // inizialmente non è possibile consumare nulla

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo MUTEX_C");
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

    for (int i = 0; i < NUM_PRODUTTORI; i++) {
        fork_produttore(q, semid, 4);
    }

    for (int i = 0; i < NUM_CONSUMATORI; i++) {
        fork_consumatore(q, semid, 8);
    }

    // il padre attende la terminazione dei produttori e dei consumatori
    for (int i = 0; i < NUM_PRODUTTORI + NUM_CONSUMATORI; i++) {
        wait(NULL);
    }

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