#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    key_t key_comandi_macchina = ftok(".", 'c');
    int id_comandi_macchina = msgget(key_comandi_macchina, IPC_CREAT | 0644);
    if (id_comandi_macchina < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_log_interfaccia = ftok(".", 't');
    int id_log_interfaccia = msgget(key_log_interfaccia, IPC_CREAT | 0644);
    if (id_log_interfaccia < 0) {
        perror("Errore nella msgget");
        exit(1);
    }
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./interfaccia", "interfaccia", NULL);
        perror("Errore nella execl");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./macchina", "macchina", NULL);
        perror("Errore nella execl");
        exit(1);
    }

    for (int i = 0; i < 2; i++) {
        int status;
        wait(&status);
        if (status != 0) {
            fprintf(stderr, "Attenzione! Un processo figlio ha terminato con status %d\n", status);
        }
    }

    msgctl(id_comandi_macchina, IPC_RMID, NULL);
    msgctl(id_log_interfaccia, IPC_RMID, NULL);

    return 0;
}