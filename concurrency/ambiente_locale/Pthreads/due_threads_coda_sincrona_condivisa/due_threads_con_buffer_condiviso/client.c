#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#define NUM_MESSAGGI 4

void send_sincrona(int id_rts, int id_ots, int id_data, struct msg_data* msg);

int main() {
    key_t key_rts = ftok(".", 'r');
    int id_rts = Msgget(key_rts, 0);

    key_t key_ots = ftok(".", 'o');
    int id_ots = Msgget(key_ots, 0);

    key_t key_data = ftok(".", 'd');
    int id_data = Msgget(key_data, 0);

    pid_t pid = getpid();

    srand(pid);

    for (int i = 0; i < NUM_MESSAGGI; i++) {
        struct msg_data msg = {
            .type = pid,
            .values = { rand() % 11, rand() % 11 }
        };

        send_sincrona(id_rts, id_ots, id_data, &msg);
    }
}

void send_sincrona(int id_rts, int id_ots, int id_data, struct msg_data* msg) {
    pid_t pid = getpid();

    printf("[Client %d] Mando RTS\n", pid);
    // manda la REQUEST TO SEND
    struct msg_rts rts = {
        .type = 1,
        .pid = pid
    };
    Msgsnd(id_rts, &rts, SIZE_MSG_RTS, 0);

    printf("[Client %d] Attendo OTS...\n", pid);
    // attende un OK TO SEND destinato al suo pid
    struct msg_ots ots;
    Msgrcv(id_ots, &ots, SIZE_MSG_OTS, pid, 0);

    printf("[Client %d] Ricevuto OTS, mando dati [%d, %d]\n", pid, msg->values[0], msg->values[1]);
    // manda il messaggio sulla coda dei dati
    Msgsnd(id_data, msg, SIZE_MSG_DATA, 0);
}
