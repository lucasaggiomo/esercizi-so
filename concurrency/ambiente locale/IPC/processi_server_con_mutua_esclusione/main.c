#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shared.h"

#define TOTALE_CLIENT 4
#define TOTALE_SERVER 4

void destroy_stuff(int id_msg[], int size_msg,
                   int id_shm[], int size_shm,
                   int id_sem[], int size_sem) {
    printf("Deallocazione globale\n");

    int ret;

    // dealloca code di messaggi
    for (int i = 0; i < size_msg; i++) {
        ret = msgctl(id_msg[i], IPC_RMID, NULL);
        if (ret < 0) {
            fprintf(stderr, "Errore nella deallocazione della coda con id %d: %s\n", id_msg[i], strerror(errno));
        }
    }

    // dealloca semafori
    for (int i = 0; i < size_sem; i++) {
        ret = semctl(id_sem[i], 0, IPC_RMID, NULL);
        if (ret < 0) {
            fprintf(stderr, "Errore nella deallocazione del semaforo con id %d: %s\n", id_sem[i], strerror(errno));
        }
    }

    // dealloca memorie condivise
    for (int i = 0; i < size_shm; i++) {
        ret = shmctl(id_shm[i], IPC_RMID, NULL);
        if (ret < 0) {
            fprintf(stderr, "Errore nella deallocazione della memoria condivisa con id %d: %s\n", id_shm[i], strerror(errno));
        }
    }
}

int main() {
    // alloca la coda delle richieste dai client verso i server
    key_t key_msg = ftok(".", 'q');
    int id_coda_richieste = msgget(key_msg, IPC_CREAT | 0644);

    if (id_coda_richieste < 0) {
        perror("Errore nella creazione della coda di richieste");
        exit(1);
    }
    printf("Coda richiesta allocata\n");

    // alloca la memoria condivisa tra i server
    key_t key_shm = ftok(".", 's');
    int id_shared_mem = shmget(key_shm, sizeof(struct shared), IPC_CREAT | 0644);

    if (id_shared_mem < 0) {
        perror("Errore nella creazione della memoria condivisa");

        // dealloco gli elementi allocati in precedenza prima di uscire
        destroy_stuff((int[]) { id_coda_richieste }, 1,
                      NULL, 0,
                      NULL, 0);
        exit(1);
    }
    printf("Memoria condivisa allocata\n");

    // effettua l'attach della memoria condivisa
    struct shared* shm = shmat(id_shared_mem, NULL, 0);
    if (shm == (void*)-1) {
        perror("Errore nell'attach della memoria condivisa");

        // dealloco gli elementi allocati in precedenza prima di uscire
        destroy_stuff((int[]) { id_coda_richieste }, 1,
                      (int[]) { id_shared_mem }, 1,
                      NULL, 0);
        exit(1);
    }
    printf("Memoria condivisa attached\n");

    // alloca il mutex per l'accesso alla memoria condivisa tra i server
    int id_mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);

    if (id_mutex < 0) {
        perror("Errore nella creazione del mutex");

        // dealloco gli elementi allocati in precedenza prima di uscire
        destroy_stuff((int[]) { id_coda_richieste }, 1,
                      (int[]) { id_shared_mem }, 1,
                      NULL, 0);
        exit(1);
    }
    printf("Mutex allocato\n");

    // inizializza il mutex e la memoria condivisa
    int child_count = 0;

    int ret = semctl(id_mutex, 0, SETVAL, 1);

    if (ret < 0) {
        perror("Errore nell'inizializzazione del mutex");
        goto ABORT;
    }

    shm->id_mutex = id_mutex;

    printf("Inizializzazione mutex e memoria condivisa completata\n");

    // CREAZIONE PROCESSI

    pid_t pid;

    // crea i processi client

    for (int i = 0; i < TOTALE_CLIENT; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella creazione di un client");

            goto ABORT;

        } else if (pid == 0) {
            execl("./client", "client", NULL);

            perror("Errore nell'exec del client");
            exit(1);
        }

        child_count++;
    }

    // crea i processi server
    for (int i = 0; i < TOTALE_SERVER; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella creazione di un client");

            goto ABORT;

        } else if (pid == 0) {
            execl("./server", "server", NULL);

            perror("Errore nell'exec del client");
            exit(1);
        }

        child_count++;
    }

ABORT:
    // attende la terminazione dei figli
    printf("Attendo la terminazione dei client e dei server...\n");
    while (child_count > 0) {
        wait(NULL);
        child_count--;
    }

    // dealloca tutto
    destroy_stuff((int[]) { id_coda_richieste }, 1,
                  (int[]) { id_shared_mem }, 1,
                  (int[]) { id_mutex }, 1);
    return 0;
}