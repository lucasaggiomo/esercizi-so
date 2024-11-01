#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../semaphore.h"

// definizione indici dei semafori nell'array dei semafori condiviso
#define SPAZIO_DISP 0  // spazio disponibile
#define MSG_DISP 1     // messaggio disponibile
#define MUTEX_P 2      // gestisce la mutua esclusione dei produttori
#define MUTEX_C 3      // gestisce la mutua esclusione dei consumatori

#define DIM 3

#define NUM_PRODUTTORI 3
#define NUM_CONSUMATORI 2

#define LIBERO 0
#define IN_USO 1
#define PIENO 2

// struct per la coda circolare
struct shared {
    int buffer[DIM];
    int status[DIM];
};

void produttore(struct shared* shm, int semid) {
    int indice = 0;

    // decrementa il semaforo SPAZIO_DISP e si blocca se il valore diventa negativo
    Wait_Sem(semid, SPAZIO_DISP);

    // mutua esclusione tra i produttori per gestire il caso di più produttori (decrementa il semaforo MUTEX_P, se è < 0 si blocca)
    Wait_Sem(semid, MUTEX_P);

    while (indice < DIM && shm->status[indice] != LIBERO) {
        indice++;
    }
    shm->status[indice] = IN_USO;

    // termina la mutua esclusione (incrementa il semaforo MUTEX_P, se è <= 0 allora sveglia un altro produttore in attesa)
    Signal_Sem(semid, MUTEX_P);

    // produce in shm->buffer[indice]
    int value = rand() % 16;
    shm->buffer[indice] = value;
    printf("Produzione:\t%d\t(indice = %d)\n", value, indice);
    // sleep(1);

    shm->status[indice] = PIENO;

    // incrementa il semaforo MSG_DISP in modo da svegliare il consumatore in coda
    Signal_Sem(semid, MSG_DISP);
}

void consumatore(struct shared* shm, int semid) {
    int indice = 0;

    // decrementa il semaforo MSG_DISP e si blocca se il valore diventa negativo
    Wait_Sem(semid, MSG_DISP);

    // Mutua esclusione per il consumatore (analogo al produttore)
    Wait_Sem(semid, MUTEX_C);

    while (indice < DIM && shm->status[indice] != PIENO) {
        indice++;
    }
    shm->status[indice] = IN_USO;

    Signal_Sem(semid, MUTEX_C);

    int value = shm->buffer[indice];
    printf("Consumatore:\t%d\t(indice = %d)\n", value, indice);

    shm->status[indice] = LIBERO;

    // incrementa il semaforo SPAZIO_DISP in modo da svegliare il produttore in coda
    Signal_Sem(semid, SPAZIO_DISP);
}

void fork_produttore(struct shared* shm, int semid, int produzioni) {
    int pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del produttore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        srand(getpid());
        // processo figlio

        for (int i = 0; i < produzioni; i++) {
            produttore(shm, semid);
        }

        // termina l'esecuzione del produttore
        exit(EXIT_SUCCESS);
    }
}

void fork_consumatore(struct shared* shm, int semid, int consumazioni) {
    int pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del consumatore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        for (int i = 0; i < consumazioni; i++) {
            consumatore(shm, semid);
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

    int ds_shm = shmget(IPC_PRIVATE, sizeof(struct shared), IPC_CREAT | 0644);

    if (ds_shm < 0) {
        perror("Errore nella creazione della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // collego ("attach") il segmento di memoria allo spazio di indirizzamento del chiamante
    struct shared* shm = shmat(ds_shm, NULL, 0);

    if (shm == (void*)-1) {
        perror("Errore nel colllegamento della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // inizializzo status, inizialmente sono tutti LIBERO
    // nota: non metto sizeof(int), in modo da adattarsi al tipo di shm->status (utile nel caso il tipo di status cambiasse)
    memset(&(shm->status), LIBERO, DIM * sizeof(*shm->status));

    for (int i = 0; i < NUM_PRODUTTORI; i++) {
        fork_produttore(shm, semid, 4);
    }

    for (int i = 0; i < NUM_CONSUMATORI; i++) {
        fork_consumatore(shm, semid, NUM_PRODUTTORI * 4 / NUM_CONSUMATORI);
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