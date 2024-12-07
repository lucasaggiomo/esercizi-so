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

struct msg_comando {
    long type;
};

struct msg_feedback {
    long type;
    int data;
};

#define DIM_LOG_MESSAGE 50
struct msg_log {
    long type;
    char message[50];
};

#define AVVIO_PREPARAZIONE 1
#define AVVIO_ROTAZIONE 2
#define INTERRUZIONE_ROTAZIONE 3
#define AVVIO_RISCALDAMENTO 4
#define INTERRUZIONE_RISCALDAMENTO 5
#define TERMINAZIONE 6

#define SIZE_MSG_RTS (sizeof(struct msg_rts) - sizeof(long))
#define SIZE_MSG_OTS (sizeof(struct msg_ots) - sizeof(long))
#define SIZE_MSG_COMANDO (sizeof(struct msg_comando) - sizeof(long))
#define SIZE_MSG_FEEDBACK (sizeof(struct msg_feedback) - sizeof(long))
#define SIZE_MSG_LOG (sizeof(struct msg_log) - sizeof(long))

void send_sincrona(int id_rts, int id_ots, int id_messaggi, void* msg, size_t size_msg);
int receive_sincrona(int id_rts, int id_ots, int id_messaggi, void* msg, size_t size_msg, int rts_msgflg);

#endif