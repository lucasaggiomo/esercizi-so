#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#define NUM_CLIENT 3

int main() {
    key_t key_rts = ftok(".", 'r');
    int id_rts = Msgget(key_rts, IPC_CREAT | 0644);

    key_t key_ots = ftok(".", 'o');
    int id_ots = Msgget(key_ots, IPC_CREAT | 0644);

    key_t key_data = ftok(".", 'd');
    int id_data = Msgget(key_data, IPC_CREAT | 0644);

    pid_t pid;

    for (int i = 0; i < NUM_CLIENT; i++) {
        pid = Fork();
        if (pid > 0)
            continue;

        Execl("./client");
        exit(1);     // mai eseguito, l'errore è gestito in Execl
    }

    pid = Fork();
    if (pid == 0) {
        Execl("./server");
        exit(1);     // mai eseguito, l'errore è gestito in Execl
    }

    for (int i = 0; i < NUM_CLIENT + 1; i++) {
        wait(NULL);
    }

    Msgctl(id_rts, IPC_RMID, NULL);
    Msgctl(id_ots, IPC_RMID, NULL);
    Msgctl(id_data, IPC_RMID, NULL);
}