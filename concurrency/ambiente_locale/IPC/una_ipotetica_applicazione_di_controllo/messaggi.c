#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"

void send_sincrona(int id_rts, int id_ots, int id_messaggi, void* msg, size_t size_msg) {
    int ret;

    // invia un rts
    struct msg_rts rts = {
        .type = 1
    };

    // printf("[Controllore] Invio REQUEST TO SEND\n");
    ret = msgsnd(id_rts, &rts, SIZE_MSG_RTS, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }

    // attende un ots
    struct msg_ots ots;
    ret = msgrcv(id_ots, &ots, SIZE_MSG_OTS, 0, 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }

    // printf("[Controllore] Ho ricevuto OK TO SEND, invio il comando\n");

    // invia il messaggio
    ret = msgsnd(id_messaggi, msg, size_msg, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }
}

int receive_sincrona(int id_rts, int id_ots, int id_messaggi, void* msg, size_t size_msg, int rts_msgflg) {
    int ret;

    // attende un rts
    struct msg_rts rts;
    ret = msgrcv(id_rts, &rts, SIZE_MSG_RTS, 0, rts_msgflg);
    if (ret < 0) {
        if (errno != ENOMSG) {
            perror("Errore nella msgrcv");
            exit(1);
        }
        // REQUEST TO SEND NON PRESENTE
        return -1;
    }

    // invia un ots
    struct msg_ots ots = { .type = 1 };
    ret = msgsnd(id_ots, &ots, SIZE_MSG_OTS, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }

    // riceve il messaggio
    ret = msgrcv(id_messaggi, msg, size_msg, 0, 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }

    return 0;
}