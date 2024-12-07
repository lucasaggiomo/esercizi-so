#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "procedure.h"

#define NUM_CLIENT 5
#define NUM_SERVER 3

#define NUM_RICHIESTE_PER_CLIENT 5

int main() {
    int id_rts = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    if (id_rts < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    int id_ots = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    if (id_ots < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    int id_richieste_interfaccia = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    if (id_richieste_interfaccia < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    int id_servers[NUM_SERVER];
    for (int i = 0; i < NUM_SERVER; i++) {
        id_servers[i] = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
        if (id_servers[i] < 0) {
            perror("Errore nella msgget");
            exit(1);
        }
    }

    pid_t pid;

    for (int i = 0; i < NUM_CLIENT; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            client(id_rts, id_ots, id_richieste_interfaccia, NUM_RICHIESTE_PER_CLIENT);
            exit(0);
        }
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        interfaccia(id_rts, id_ots, id_richieste_interfaccia, id_servers, NUM_SERVER);
        exit(0);
    }

    for (int i = 0; i < NUM_SERVER; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            server(id_servers[i]);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_CLIENT; i++) {
        int status;
        wait(&status);
        if (status != 0) {
            fprintf(stderr, "Attenzione! Un client ha terminato con status %d\n", status);
        }
    }

    // invia messaggio di terminazione all'interfaccia (come REQUEST TO SEND)
    struct msg_rts terminazione_interfaccia = {
        .type = TERMINA_PROCESSO,
        .pid = getpid()
    };
    printf("[MASTER %d] Invio messaggio di terminazione all'interfaccia\n", getpid());
    int ret = msgsnd(id_rts, &terminazione_interfaccia, SIZE_MSG_RTS, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd di terminazione");
        exit(1);
    }

    // invia messaggio di terminazione ai server (come richieste)
    struct msg_richiesta terminazione_server = {
        .type = TERMINA_PROCESSO,
        .pid_client = getpid(),
        .value = -1
    };
    for (int i = 0; i < NUM_SERVER; i++) {
        printf("[MASTER %d] Invio messaggio di terminazione al server %d\n", getpid(), i);
        int ret = msgsnd(id_servers[i], &terminazione_server, SIZE_MSG_RICHIESTA, 0);
        if (ret < 0) {
            perror("Errore nella msgsnd di terminazione");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_SERVER + 1; i++) {
        int status;
        wait(&status);
        if (status != 0) {
            fprintf(stderr, "Attenzione! Un processo figlio (server o interfaccia) ha terminato con status %d\n", status);
        }
    }

    msgctl(id_rts, IPC_RMID, NULL);
    msgctl(id_ots, IPC_RMID, NULL);
    msgctl(id_richieste_interfaccia, IPC_RMID, NULL);
    for (int i = 0; i < NUM_SERVER; i++) {
        msgctl(id_servers[i], IPC_RMID, NULL);
    }

    return 0;
}