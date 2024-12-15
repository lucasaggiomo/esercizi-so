#ifndef MESSAGGI_H
#define MESSAGGI_H

#include <sys/ipc.h>
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

struct msg_risposta {
    long type;
    int length;
    key_t key_shm;
};

#define RICHIESTA_SHM 1
#define TERMINA_SHM 2
#define TERMINA_PROCESSO 3

#define SIZEOF_MSG(msg) (sizeof(msg) - sizeof(long))

#endif