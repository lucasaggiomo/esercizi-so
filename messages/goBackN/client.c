#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "app_client.h"
#include "transport_client.h"

void init_queues(int* id_rts_ots, int* id_app_dati) {
    // alloca una coda per l'invio dei messaggi RTS e OTS tra app e trasporto
    key_t key_rts_ots = ftok(".", 'r');
    *id_rts_ots = msgget(key_rts_ots, IPC_CREAT | 0644);
    if (*id_rts_ots < 0) {
        perror("[Client] Errore nella msgget della coda rts - ots del client");
        exit(1);
    }

    // alloca una coda per l'invio dei messaggi app al livello trasporto del client
    key_t key_app_dati = ftok(".", 'c');
    *id_app_dati = msgget(key_app_dati, IPC_CREAT | 0644);
    if (*id_app_dati < 0) {
        perror("[Client] Errore nella msgget dei dati dell'app verso il trasporto del client");
        exit(1);
    }
}

void destroy_queues(int id_rts_ots, int id_app_dati) {
    int ret;
    // dealloca le code
    ret = msgctl(id_rts_ots, IPC_RMID, NULL);
    if (ret < 0) {
        perror("[Client] Errore nella deallocazione della coda rts - ots del client");
    }
    ret = msgctl(id_app_dati, IPC_RMID, NULL);
    if (ret < 0) {
        perror("[Client] Errore nella deallocazione della coda dei dati app-trasporto del client");
    }
}

void fork_exec_children(int id_rts_ots, int id_app_dati) {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("[Client] Errore nella fork client");
        exit(1);
    } else if (pid == 0) {
        srand(getpid());
        app_client(id_rts_ots, id_app_dati);
        exit(0);
    }

    pid = fork();
    if (pid < 0) {
        perror("[Client] Errore nella fork client");
        exit(1);
    } else if (pid == 0) {
        srand(getpid());
        transport_client(id_rts_ots, id_app_dati);
        exit(0);
    }
}

int main() {
    int id_rts_ots;
    int id_app_dati;

    printf("[Client] In esecuzione\n");

    // alloca le code per la comunicazione app <-> trasporto
    init_queues(&id_rts_ots, &id_app_dati);

    // creazione processi figli (livello applicativo e trasporto)
    fork_exec_children(id_rts_ots, id_app_dati);

    // attende la terminazione
    wait(NULL);
    wait(NULL);

    // dealloca le code per la comunicazione app <-> trasporto
    destroy_queues(id_rts_ots, id_app_dati);

    printf("[Client] Terminazione\n");

    return 0;
}