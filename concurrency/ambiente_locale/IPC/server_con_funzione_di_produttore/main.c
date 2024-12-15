#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUMERO_RICHIESTE 2

int main() {
    key_t key_comandi = ftok(".", 'c');
    int id_comandi = msgget(key_comandi, IPC_CREAT | 0644);
    if (id_comandi < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_risposte = ftok(".", 'p');
    int id_risposte = msgget(key_risposte, IPC_CREAT | 0644);
    if (id_risposte < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_rts_comandi = ftok(".", 'r');
    int id_rts_comandi = msgget(key_rts_comandi, IPC_CREAT | 0644);
    if (id_rts_comandi < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_ots_comandi = ftok(".", 'r');
    int id_ots_comandi = msgget(key_ots_comandi, IPC_CREAT | 0644);
    if (id_ots_comandi < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        char number[21];
        snprintf(number, sizeof(number), "%d", NUMERO_RICHIESTE);
        execl("./client", "client", number, NULL);
        perror("Errore nella execl");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./server", "server", NULL);
        perror("Errore nella execl");
        exit(1);
    }

    wait(NULL);
    wait(NULL);

    msgctl(id_risposte, IPC_RMID, NULL);
    msgctl(id_comandi, IPC_RMID, NULL);
    msgctl(id_rts_comandi, IPC_RMID, NULL);
    msgctl(id_ots_comandi, IPC_RMID, NULL);

    return 0;
}