#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"

void send_sincrona(int id_rts, int id_ots, int id_richieste, struct msg_richiesta* richiesta) {
    int ret;

    // invia un rts
    struct msg_rts rts = {
        .type = RICHIESTA_CLIENT,
        .pid = getpid()
    };

    printf("[Client %d] Invio REQUEST TO SEND\n", getpid());
    ret = msgsnd(id_rts, &rts, SIZE_MSG_RTS, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }

    // attende un ots
    struct msg_ots ots;
    ret = msgrcv(id_ots, &ots, SIZE_MSG_OTS, getpid(), 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }

    printf("[Client %d] Ho ricevuto OK TO SEND, invio la richiesta [%d, %d]\n", getpid(), richiesta->pid_client, richiesta->value);

    // invia la richiesta
    ret = msgsnd(id_richieste, richiesta, SIZE_MSG_RICHIESTA, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }
}