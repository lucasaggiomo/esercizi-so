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

struct node {
    int index;
    struct node* next;
};

#define DIM 3

struct shared {
    int buffer[DIM];
    struct node* indici_liberi;
    struct node* indici_pieni;
};

#define TOTALE_PRODUTTORI 5
#define TOTALE_CONSUMATORI 3

#define TOTALE_PRODUZIONI 3
#define TOTALE_CONSUMI 5

#define SPAZIO_DISP 0
#define MESSAGGIO_DISP 1
#define MUTEX_P_LIBERI 2
#define MUTEX_P_PIENI 3
#define MUTEX_C_LIBERI 4
#define MUTEX_C_PIENI 5

void produttore(int id_sem, struct shared* shm) {
    struct node* libero;
    Wait_Sem(id_sem, SPAZIO_DISP);

    Wait_Sem(id_sem, MUTEX_P_LIBERI);

    // seleziono l'indice libero e lo rimuovo dalla lista degli indici liberi
    libero = shm->indici_liberi;
    shm->indici_liberi = shm->indici_liberi->next;

    Signal_Sem(id_sem, MUTEX_P_LIBERI);

    printf("[Produttore %d] Ho trovato l'elemento libero all'indice %d\n", getpid(), libero->index);

    // produzione
    sleep(1);

    shm->buffer[libero->index] = rand() % 100;

    printf("[Produttore %d] Ho prodotto %d in %d\n", getpid(), shm->buffer[libero->index], libero->index);

    Wait_Sem(id_sem, MUTEX_P_PIENI);

    // imposto il nodo in cui ho prodotto come testa degli indici pieni
    libero->next = shm->indici_pieni;
    shm->indici_pieni = libero;

    Signal_Sem(id_sem, MUTEX_P_PIENI);

    Signal_Sem(id_sem, MESSAGGIO_DISP);
}

void consumatore(int id_sem, struct shared* shm) {
    struct node* pieno;
    Wait_Sem(id_sem, MESSAGGIO_DISP);

    Wait_Sem(id_sem, MUTEX_C_PIENI);

    // seleziono l'indice pieno e lo rimuovo dalla lista degli indici pieni
    pieno = shm->indici_pieni;
    shm->indici_pieni = shm->indici_pieni->next;

    Signal_Sem(id_sem, MUTEX_C_PIENI);

    printf("[Consumatore %d] Ho trovato l'elemento pieno all'indice %d\n", getpid(), pieno->index);

    // consumo
    int value = shm->buffer[pieno->index];

    printf("[Consumatore %d] Ho consumato %d in %d\n", getpid(), value, pieno->index);

    Wait_Sem(id_sem, MUTEX_C_LIBERI);

    // imposto il nodo in cui ho consumato come testa degli indici liberi
    pieno->next = shm->indici_liberi;
    shm->indici_liberi = pieno;

    Signal_Sem(id_sem, MUTEX_C_LIBERI);

    Signal_Sem(id_sem, SPAZIO_DISP);
}

int main() {
    // alloco i semafori necessari
    int id_sem = semget(IPC_PRIVATE, 6, IPC_CREAT | 0644);

    if (id_sem < 0) {
        perror("Errore nell'allocazione dei semafori");
        exit(1);
    }

    printf("Semafori allocati con successo\n");

    // inizializzo i semafori
    semctl(id_sem, SPAZIO_DISP, SETVAL, DIM);
    semctl(id_sem, MESSAGGIO_DISP, SETVAL, 0);
    semctl(id_sem, MUTEX_P_LIBERI, SETVAL, 1);
    semctl(id_sem, MUTEX_P_PIENI, SETVAL, 1);
    semctl(id_sem, MUTEX_C_LIBERI, SETVAL, 1);
    semctl(id_sem, MUTEX_C_PIENI, SETVAL, 1);

    printf("Semafori inizializzati con successo\n");

    // alloco la memoria condivisa
    int id_shm = shmget(IPC_PRIVATE, sizeof(struct shared), IPC_CREAT | 0644);

    if (id_sem < 0) {
        perror("Errore nell'allocazione della memoria condivisa");
        exit(1);
    }

    printf("Memoria condivisa allocata con successo\n");

    // effettuo l'attach
    struct shared* shm = shmat(id_shm, NULL, 0);

    if (shm == (void*)-1) {
        perror("Errore nell'attach");
        exit(1);
    }

    printf("Memoria condivisa attached con successo\n");

    // inizializzo la memoria condivisa

    // in particolare alloco DIM nodi di tipo struct node, inizialmente tutti liberi, uno per ogni indice di buffer, come memoria condivisa
    int id_nodes = shmget(IPC_PRIVATE, DIM * sizeof(struct node), IPC_CREAT | 0644);

    if (id_nodes < 0) {
        perror("Errore nell'allocazione dei nodi");
        exit(1);
    }

    struct node* nodes = shmat(id_nodes, NULL, 0);

    if (nodes == (void*)-1) {
        perror("Errore nell'attach dei nodi");
        exit(1);
    }

    // creo la linked list
    struct node* curr = &nodes[0];
    for (int i = 0; i < DIM - 1; i++) {
        curr->index = i;
        curr->next = &nodes[i + 1];
        curr = curr->next;
    }
    curr->index = DIM - 1;
    curr->next = NULL;

    shm->indici_liberi = &nodes[0];
    shm->indici_pieni = NULL;

    printf("Memoria condivisa inizializzata con successo\n");

    // creo i processi produttori e consumatori
    for (int i = 0; i < TOTALE_PRODUTTORI; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Errore nella creazione di un processo produttore");
            exit(1);
        } else if (pid == 0) {
            srand(getpid());
            for (int j = 0; j < TOTALE_PRODUZIONI; j++) {
                produttore(id_sem, shm);
                sleep(1);
            }
            exit(0);
        }
    }

    printf("Produttori creati con successo\n");

    for (int i = 0; i < TOTALE_CONSUMATORI; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Errore nella creazione di un processo produttore");
            exit(1);
        } else if (pid == 0) {
            for (int j = 0; j < TOTALE_CONSUMI; j++) {
                consumatore(id_sem, shm);
                sleep(2);
            }
            exit(0);
        }
    }

    printf("Consumatori creati con successo\n");

    // attendo la terminazione dei processi
    for (int i = 0; i < TOTALE_PRODUTTORI + TOTALE_CONSUMATORI; i++) {
        wait(NULL);
    }

    printf("Attesa dei figli completata con successo\n");

    // dealloco i semafori
    semctl(id_sem, 0, IPC_RMID, NULL);

    // dealloco la memoria condivisa
    shmctl(id_shm, IPC_RMID, NULL);
    shmctl(id_nodes, IPC_RMID, NULL);

    printf("Terminazione\n");
}