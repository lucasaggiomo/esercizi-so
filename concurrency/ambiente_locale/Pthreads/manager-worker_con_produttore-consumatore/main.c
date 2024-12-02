#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#define NUM_CLIENT 3

int main() {
    key_t key_richeste = ftok(".", 'q');
    int id_richieste = Msgget(key_richeste, IPC_CREAT | 0644);

    key_t key_risposte = ftok(".", 'a');
    int id_risposte = Msgget(key_risposte, IPC_CREAT | 0644);

    pid_t pid;

    for (int i = 0; i < NUM_CLIENT; i++) {
        pid = Fork();
        if (pid == 0) {
            Execl("./client");
            exit(1);     // mai eseguito
        }
    }

    pid = Fork();
    if (pid == 0) {
        Execl("./server");
        exit(1);     // mai eseguito
    }

    for (int i = 0; i < NUM_CLIENT; i++) {
        wait(NULL);
    }

    sleep(3);

    Msgsnd(id_richieste, &MSG_TERMINAZIONE, SIZE_MSG_RICHIESTA, 0);

    Msgctl(id_richieste, IPC_RMID, NULL);
    Msgctl(id_risposte, IPC_RMID, NULL);

    return 0;
}