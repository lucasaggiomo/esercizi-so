#ifndef SCAFFALE_H
#define SCAFFALE_H

#include <sys/types.h>

enum stato {
    LIBERO,
    OCCUPATO,
    IN_USO
};

struct scaffale {
    pid_t id_fornitore;
    enum stato stato;
};

static unsigned int livello_scorte;

#endif