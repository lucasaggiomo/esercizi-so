#ifndef MESSAGGI_H
#define MESSAGGI_H

#include <sys/types.h>

struct msg_richiesta {
    long type;
    pid_t pid;
    int data;
};

#define PID_TERMINAZIONE -1

enum response {
    CONFERMA_TRASMISSIONE,
    RIFIUTO_TRASMISSIONE
};

struct msg_risposta {
    long type;
    enum response response;
};

struct msg_rts_ots {
    long type;
};

#define REQUEST_TO_SEND 1
#define OK_TO_SEND 2

#define SIZE_MSG_RICHIESTA (sizeof(struct msg_richiesta) - sizeof(long))
#define SIZE_MSG_RISPOSTA (sizeof(struct msg_risposta) - sizeof(long))
#define SIZE_MSG_RTS_OTS (sizeof(struct msg_rts_ots) - sizeof(long))

#endif