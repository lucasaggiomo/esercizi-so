#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

void produci(buffer_t* b, int id_consumazioni, int id_produzioni, buffer_t produzione);

int main() {
    key_t key_shm = ftok(".", 's');
    int id_shm = shmget(key_shm, sizeof(buffer_t), 0);
    if (id_shm < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    buffer_t* shm = shmat(id_shm, NULL, 0);
    if (shm == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    key_t key_consumazioni = ftok(".", 'c');
    int id_consumazioni = msgget(key_consumazioni, 0);
    if (id_consumazioni < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_produzioni = ftok(".", 'p');
    int id_produzioni = msgget(key_produzioni, 0);
    if (id_produzioni < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    buffer_t b;
    for (int i = 0; i < NUM_ITERAZIONI; i++) {
        b = rand() % 10;
        produci(shm, id_consumazioni, id_produzioni, b);
    }

    return 0;
}

void produci(buffer_t* b, int id_consumazioni, int id_produzioni, buffer_t produzione) {
    int ret;
    struct msg_richiesta richiesta = { .type = 1 };

    // attende una richiesta di produzione
    ret = msgrcv(id_produzioni, &richiesta, SIZEOF_MSG(struct msg_richiesta), 0, 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }

    // effettua la produzione (scrivendo sul buffer)
    *b = produzione;
    sleep(1);

    printf("[Produttore] Ho prodotto il buffer %d\n", produzione);

    // manda la richiesta di consumazione
    ret = msgsnd(id_consumazioni, &richiesta, SIZEOF_MSG(struct msg_richiesta), 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }
}