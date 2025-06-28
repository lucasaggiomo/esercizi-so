#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"

/*
    Il processo Client (ossia il processo creato dal SO all'avvio del
    secondo eseguibile, da lanciare in un terminale separato dal primo)
    dovrà inviare una sequenza di 9 messaggi verso il Server, per poi
    terminare. Si utilizzi per la comunicazione una send di tipo asincrono.
*/

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Errore! Riesegui il programma inserendo il numero di richieste da dover inviare");
        exit(1);
    }
    int num_richieste = atoi(argv[1]);

    key_t key_richieste = ftok(".", 'r');
    int id_richieste = msgget(key_richieste, IPC_CREAT | 0644);
    if (id_richieste < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    srand(getpid());

    for (int i = 0; i < num_richieste; i++) {
        struct msg_richiesta richiesta = {
            .type = 1,
            .quantita = rand() % 10 + 1
        };

        printf("[Client] Invio richiesta con quantita %d\n", richiesta.quantita);

        int ret = msgsnd(id_richieste, &richiesta, SIZEOF_MSG(struct msg_richiesta), 0);
        if (ret < 0) {
            perror("Errore nella msgsnd");
            exit(1);
        }
    }

    printf("[Client] Terminazione\n");

    return 0;
}