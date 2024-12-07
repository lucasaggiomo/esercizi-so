#ifndef OPERANDI_RISULTATI_H
#define OPERANDI_RISULTATI_H

#include <pthread.h>

#include "buffer_stato.h"

struct operandi {
    int operando1;
    int operando2;
};

typedef struct operandi operandi_t;
typedef int risultato_t;

typedef struct buffer_stato_m operandi_m;
typedef struct buffer_stato_m risultati_m;

#endif