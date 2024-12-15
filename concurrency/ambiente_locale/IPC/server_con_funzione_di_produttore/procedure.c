#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"

#include "procedure.h"

void send_sincrona(int id_coda, int id_rts, int id_ots, void* msg, size_t size_msg) {
    int ret;
    pid_t pid = getpid();

    // invia una REQUEST TO SEND
    struct msg_rts rts = {
        .type = 1,
        .pid = pid
    };
    ret = msgsnd(id_rts, &rts, SIZEOF_MSG(struct msg_rts), 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }

    // attende una OK TO SEND
    struct msg_ots ots;
    ret = msgrcv(id_ots, &ots, SIZEOF_MSG(struct msg_ots), pid, 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }

    // invia il messaggio msg
    ret = msgsnd(id_coda, msg, size_msg, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }
}

void receive_sincrona(int id_coda, int id_rts, int id_ots, void* msg, size_t size_msg) {
    int ret;

    // attende una REQUEST TO SEND
    struct msg_rts rts;
    ret = msgrcv(id_rts, &rts, SIZEOF_MSG(struct msg_rts), 0, 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }

    // invia una OK TO SEND
    struct msg_ots ots = {
        .type = rts.pid
    };
    ret = msgsnd(id_ots, &ots, SIZEOF_MSG(struct msg_ots), 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }

    // attende il messaggio msg
    ret = msgrcv(id_coda, msg, size_msg, 0, 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }
}