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

struct msg_data {
    long type;
    int values[2];
};

#define SIZE_MSG_RTS (sizeof(struct msg_rts) - sizeof(long))
#define SIZE_MSG_OTS (sizeof(struct msg_ots) - sizeof(long))
#define SIZE_MSG_DATA (sizeof(struct msg_data) - sizeof(long))

#endif