#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

void init_queues(int* id_dati, int* id_ack) {
    // alloca la coda per l'invio dei dati (messaggi tra i livelli trasporto (client-server))
    key_t key_dati = ftok(".", 'd');
    *id_dati = msgget(key_dati, IPC_CREAT | 0644);
    if (*id_dati < 0) {
        perror("[Master] Errore nella msgget dei dati");
        exit(1);
    }

    // alloca la coda per l'invio degli ack (messaggi tra i livelli trasporto (server-client))
    key_t key_ack = ftok(".", 'a');
    *id_ack = msgget(key_ack, IPC_CREAT | 0644);
    if (*id_ack < 0) {
        perror("[Master] Errore nella msgget dell'ack");
        exit(1);
    }
}

void destroy_queues(int id_dati, int id_ack) {
    int ret;
    // dealloca le code
    ret = msgctl(id_dati, IPC_RMID, NULL);
    if (ret < 0) {
        perror("[Master] Errore nella deallocazione della coda dei dati");
    }
    ret = msgctl(id_ack, IPC_RMID, NULL);
    if (ret < 0) {
        perror("[Master] Errore nella deallocazione della coda dell'ack");
    }
}

void fork_exec_children() {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("[Master] Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./client", "client", NULL);
        perror("[Master] Errore nella execl del client");
        exit(1);
    }

    printf("[Master] Creo il processo server\n");

    pid = fork();
    if (pid < 0) {
        perror("[Master] Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./server", "server", NULL);
        perror("[Master] Errore nella execl del server");
        exit(1);
    }
}

int main() {
    int id_dati;
    int id_ack;

    printf("[Master] Alloco le code\n");

    // alloca le code per i messaggi tra client e server (che useranno i livelli trasporto)
    init_queues(&id_dati, &id_ack);

    printf("[Master] id_dati = %d, id_ack = %d\n", id_dati, id_ack);

    printf("[Master] Creo il processo client\n");

    // creazione dei processi client e server
    fork_exec_children();

    printf("[Master] Attendo la terminazione dei processi...\n");

    // attende la terminazione dei processi
    wait(NULL);
    wait(NULL);

    printf("[Master] Dealloco le code\n");

    // dealloca le code
    destroy_queues(id_dati, id_ack);

    return 0;
}