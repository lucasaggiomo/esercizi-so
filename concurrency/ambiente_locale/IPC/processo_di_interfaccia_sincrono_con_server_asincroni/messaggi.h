#ifndef MESSAGGI_H
#define MESSAGGI_H

#include <sys/types.h>

struct msg_rts {
    long type;
    pid_t pid;
};

struct msg_ots {
    long type;
};

struct msg_richiesta {
    long type;
    pid_t pid_client;
    int value;
};

#define RICHIESTA_CLIENT 1
#define TERMINA_COMUNICAZIONE 2
#define TERMINA_PROCESSO 3

#define SIZE_MSG_RTS (sizeof(struct msg_rts) - sizeof(long))
#define SIZE_MSG_OTS (sizeof(struct msg_ots) - sizeof(long))
#define SIZE_MSG_RICHIESTA (sizeof(struct msg_richiesta) - sizeof(long))

void send_sincrona(int id_rts, int id_ots, int id_richieste, struct msg_richiesta* richiesta);

#endif