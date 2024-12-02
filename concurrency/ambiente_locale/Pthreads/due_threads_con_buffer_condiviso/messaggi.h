#ifndef MESSAGGI_H
#define MESSAGGI_H

#include <sys/types.h>

struct msg_comando {
    long type;
    key_t key_coda;
};

#define INIZIO_LETTURA 1
#define FINE_LETTURA 2
#define TERMINA_SESSIONE 3

struct msg_data {
    long type;
    int value;
};

#define SIZE_MSG_COMANDO (sizeof(struct msg_comando) - sizeof(long))
#define SIZE_MSG_DATA (sizeof(struct msg_data) - sizeof(long))

#endif