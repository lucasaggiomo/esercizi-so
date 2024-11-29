#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "procedure.h"
#include "wrapper.h"

#define MAX_MESSAGES 10

#define NUM_PRODUTTORI 4
#define NUM_PRODUZIONI 5

#define NUM_CONSUMATORI 2
#define NUM_CONSUMI 10

int main() {
    int id_data = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    int id_token = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);

    struct msg_token token = {
        .type = 1
    };

    for (int i = 0; i < MAX_MESSAGES; i++) {
        Msgsnd(id_token, &token, SIZE_MSG_TOKEN, 0);
    }

    pid_t pid;

    for (int i = 0; i < NUM_PRODUTTORI; i++) {
        pid = Fork();
        if (pid == 0) {
            srand(getpid());
            for (int j = 0; j < NUM_PRODUZIONI; j++) {
                produci(id_data, id_token);
            }
            exit(0);
        }
    }

    for (int i = 0; i < NUM_CONSUMATORI; i++) {
        pid = Fork();
        if (pid == 0) {
            srand(getpid());
            for (int j = 0; j < NUM_CONSUMI; j++) {
                sleep(1);
                consuma(id_data, id_token);
            }
            exit(0);
        }
    }

    for (int i = 0; i < NUM_PRODUTTORI + NUM_CONSUMATORI; i++) {
        wait(NULL);
    }

    Msgctl(id_data, IPC_RMID, NULL);
    Msgctl(id_token, IPC_RMID, NULL);
}