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

#include "buffer.h"

#define TOTALE_PRODUTTORI 3
#define TOTALE_CONSUMATORI 1

#define TOTALE_PRODUZIONI 3

#define TOTALE_INIEZIONI 9

#define COPY_CHANCE 50
#define CORRUPT_VALUE_CHANCE 30
#define CORRUPT_STRING_CHANCE (100 - COPY_CHANCE - CORRUPT_VALUE_CHANCE)

void produttore(int id_sem, struct buffer* b, const char* msg) {
    srand(getpid());

    struct buffer source;
    strcpy(source.str, msg);
    source.value = strlen(msg);

    for (int i = 0; i < TOTALE_PRODUZIONI; i++) {
        // modifica l'ID ad ogni produzione, per avere buffer distinguibili
        source.ID = rand() % 100;

        // produce in mutua esclusione
        produci(id_sem, b, &source);

        printf("[Produttore %d] Ho prodotto il buffer [%d] \"%s\" (%d)\n", getpid(), source.ID, source.str, source.value);
    }
}

void iniettore(int id_sem_p, int id_sem_c, struct buffer* b_p, struct buffer* b_c) {
    srand(getpid());

    struct buffer buffer_consumato;
    struct buffer buffer_da_produrre;

    for (int i = 0; i < TOTALE_INIEZIONI; i++) {
        // consuma in mutua esclusione il buffer del produttore
        consuma(id_sem_p, &buffer_consumato, b_p);

        printf("[Iniettore %d] Ho consumato il buffer [%d] \"%s\" (%d)\n", getpid(), buffer_consumato.ID, buffer_consumato.str, buffer_consumato.value);

        memcpy(&buffer_da_produrre, &buffer_consumato, sizeof(struct buffer));

        int n = rand() % 100;

        if (n < COPY_CHANCE) {
            // Copia senza modificare

        } else if (n < COPY_CHANCE + CORRUPT_VALUE_CHANCE) {
            // Corrompe il valore intero
            buffer_da_produrre.value = rand();
            printf("[Iniettore %d] CORRUZIONE DEL VALORE INTERO DI [%d]\n", getpid(), buffer_consumato.ID);
        } else {
            // Corrompe la stringa
            int rand_index = rand() % strlen(buffer_da_produrre.str);     // source->value dovrebbe essere strlen(source->str)
            buffer_da_produrre.str[rand_index] = '\0';
            printf("[Iniettore %d] CORRUZIONE DELLA STRINGA DI [%d]\n", getpid(), buffer_consumato.ID);
        }

        // produce in mutua esclusione il buffer del consumatore
        produci(id_sem_c, b_c, &buffer_da_produrre);

        printf("[Iniettore %d] Ho prodotto il buffer [%d] \"%s\" (%d)\n", getpid(), buffer_da_produrre.ID, buffer_da_produrre.str, buffer_da_produrre.value);
    }
}

void consumatore(int id_sem, struct buffer* b) {
    struct buffer buffer_consumato;

    for (int i = 0; i < TOTALE_INIEZIONI; i++) {
        // consuma in mutua esclusione
        consuma(id_sem, &buffer_consumato, b);

        // verifica se è corrotto
        int len = strlen(buffer_consumato.str);
        if (buffer_consumato.value == len) {
            printf("[Consumato %d] Ho consumato il buffer NON CORROTTO [%d] \"%s\" (%d)\n", getpid(), buffer_consumato.ID, buffer_consumato.str, buffer_consumato.value);
        } else {
            printf("[Consumato %d] Ho consumato il buffer CORROTTO [%d] \"%s\" (%d)\n", getpid(), buffer_consumato.ID, buffer_consumato.str, buffer_consumato.value);
        }
    }
}

int main() {
    int id_sem[2];

    for (int i = 0; i < 2; i++) {
        id_sem[i] = semget(IPC_PRIVATE, 3, IPC_CREAT | 0644);

        if (id_sem[i] < 0) {
            perror("Errore nella semget");
            exit(1);
        }

        semctl(id_sem[i], PRODUTTORE, SETVAL, 1);
        semctl(id_sem[i], CONSUMATORE, SETVAL, 0);
    }

    int id_shm_p = shmget(IPC_PRIVATE, sizeof(struct buffer), IPC_CREAT | 0644);

    if (id_shm_p < 0) {
        perror("Errore nella shmget del buffer del produttore");
        exit(1);
    }

    struct buffer* b_p = shmat(id_shm_p, NULL, 0);

    if (b_p == (void*)-1) {
        perror("Errore nella shmat del buffer del produttore");
        exit(1);
    }

    int id_shm_c = shmget(IPC_PRIVATE, sizeof(struct buffer), IPC_CREAT | 0644);

    if (id_shm_c < 0) {
        perror("Errore nella shmget del buffer del consumatore");
        exit(1);
    }

    struct buffer* b_c = shmat(id_shm_c, NULL, 0);

    if (b_c == (void*)-1) {
        perror("Errore nella shmat del buffer del consumatore");
        exit(1);
    }

    pid_t pid;
    for (int i = 0; i < TOTALE_PRODUTTORI; i++) {
        pid = fork();

        if (pid < 0) {
            perror("Errore nella creazione di un produttore");
            exit(1);
        } else if (pid == 0) {
            char msg[5] = "MSG";
            sprintf(msg, "MSG%d", i + 1);
            produttore(id_sem[0], b_p, msg);
            exit(0);
        }
    }

    for (int i = 0; i < TOTALE_CONSUMATORI; i++) {
        pid = fork();

        if (pid < 0) {
            perror("Errore nella creazione di un consumatore");
            exit(1);
        } else if (pid == 0) {
            consumatore(id_sem[1], b_c);
            exit(0);
        }
    }

    pid = fork();

    if (pid < 0) {
        perror("Errore nella creazione dell'iniettore");
        exit(1);
    } else if (pid == 0) {
        iniettore(id_sem[0], id_sem[1], b_p, b_c);
        exit(0);
    }

    for (int i = 0; i < TOTALE_CONSUMATORI + TOTALE_PRODUTTORI + 1; i++) {
        wait(NULL);
    }

    for (int i = 0; i < 2; i++) {
        semctl(id_sem[i], 0, IPC_RMID, NULL);
    }
    shmctl(id_shm_p, IPC_RMID, NULL);
    shmctl(id_shm_c, IPC_RMID, NULL);

    return 0;
}