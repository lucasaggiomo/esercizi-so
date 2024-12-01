#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

int main() {
    key_t key_comandi = ftok(".", 'r');
    int id_comandi = Msgget(key_comandi, IPC_CREAT | 0644);

    pid_t pid;

    // Client
    pid = Fork();
    if (pid == 0) {
        Execl("./client");
        exit(1);     // mai eseguito, l'errore è gestito in Execl
    }

    // Server
    pid = Fork();
    if (pid == 0) {
        Execl("./server");
        exit(1);     // mai eseguito, l'errore è gestito in Execl
    }

    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    Msgctl(id_comandi, IPC_RMID, NULL);
}