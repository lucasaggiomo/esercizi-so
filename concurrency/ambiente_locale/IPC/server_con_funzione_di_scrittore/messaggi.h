#ifndef MESSAGGI_H
#define MESSAGGI_H

#include <sys/ipc.h>
#include <sys/types.h>

#include "aeroporto.h"

struct msg_avviso {
    long type;
    int numero_gate;
    struct gate gate;
};

#define SIZEOF_MSG(msg) (sizeof(msg) - sizeof(long))

#endif