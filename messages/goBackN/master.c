#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int ret;

    printf("[Master] Alloco le code\n");

    // alloca la coda per l'invio dei dati (messaggi tra i livelli trasporto (client-server))
    key_t key_dati = ftok(".", 'd');
    int id_dati = msgget(key_dati, IPC_CREAT | 0644);
    if (id_dati < 0) {
        perror("Errore nella msgget dei dati");
        exit(1);
    }

    // alloca la coda per l'invio degli ack (messaggi tra i livelli trasporto (server-client))
    key_t key_ack = ftok(".", 'a');
    int id_ack = msgget(key_ack, IPC_CREAT | 0644);
    if (id_ack < 0) {
        perror("Errore nella msgget dell'ack");
        exit(1);
    }

    printf("[Master] Creo il processo client\n");

    // creazione dei processi client e server
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./app_client", "app_client", NULL);
        perror("Errore nella execl del client");
        exit(1);
    }

    printf("[Master] Creo il processo server\n");

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./app_server", "app_server", NULL);
        perror("Errore nella execl del server");
        exit(1);
    }

    printf("[Master] Attendo la terminazione dei processi...\n");

    // attende la terminazione dei processi
    wait(NULL);
    wait(NULL);

    printf("[Master] Dealloco le code\n");

    // dealloca le code
    ret = msgctl(id_dati, IPC_RMID, NULL);
    if (ret < 0) {
        perror("Errore nella deallocazione della coda dei dati");
    }
    ret = msgctl(id_ack, IPC_RMID, NULL);
    if (ret < 0) {
        perror("Errore nella deallocazione della coda dell'ack");
    }

    return 0;
}