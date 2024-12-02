#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "app_server.h"
#include "transport_server.h"

void init_queues(int* id_app_dati) {
    // alloca una coda per la ricezione dei messaggi creati dall'app client, che vengono inviati dal livello trasporto server all'app server
    key_t key_app_dati = ftok(".", 's');
    *id_app_dati = msgget(key_app_dati, IPC_CREAT | 0644);
    if (*id_app_dati < 0) {
        perror("[Server] Errore nella msgget dei messaggi app da trasporto all'app del server");
        exit(1);
    }
}

void destroy_queues(int id_app_dati) {
    // dealloca le code
    int ret = msgctl(id_app_dati, IPC_RMID, NULL);
    if (ret < 0) {
        perror("[Server] Errore nella deallocazione della coda dei dati app-trasporto del server");
    }
}

void fork_exec_children(int id_app_dati) {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("[Server] Errore nella fork server");
        exit(1);
    } else if (pid == 0) {
        srand(getpid());
        app_server(id_app_dati);
        exit(0);
    }

    pid = fork();
    if (pid < 0) {
        perror("[Server] Errore nella fork server");
        exit(1);
    } else if (pid == 0) {
        srand(getpid());
        transport_server(id_app_dati);
        exit(0);
    }
}

int main() {
    int id_app_dati;

    printf("[Server] In esecuzione\n");

    // alloca la coda per i messaggi tra livello applicativo e trasporto
    init_queues(&id_app_dati);

    // creazione processi figli (livello applicativo e trasporto)
    fork_exec_children(id_app_dati);

    // attende la terminazione
    wait(NULL);
    wait(NULL);

    // dealloca la coda
    destroy_queues(id_app_dati);

    printf("[Server] Terminazione\n");

    return 0;
}