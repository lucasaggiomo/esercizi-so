#ifndef MESSAGGI_H
#define MESSAGGI_H

#include <sys/types.h>

struct msg_richiesta {
    long type;
    pid_t pid;
    int values[2];
};

struct msg_risposta {
    long type;
    int result;
};

static const struct msg_richiesta MSG_TERMINAZIONE = {
    .type = 1,
    .values = { -1, -1 }
};

#define SIZE_MSG_RICHIESTA (sizeof(struct msg_richiesta) - sizeof(long))
#define SIZE_MSG_RISPOSTA (sizeof(struct msg_risposta) - sizeof(long))

#endif