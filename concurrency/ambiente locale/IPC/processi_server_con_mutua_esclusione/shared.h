#ifndef _SHARED_H
#define _SHARED_H

#include "data.h"

struct shared {
    int id_mutex;
    struct data data;
};

void update_shared(struct shared* shm, struct data* new_data);

#endif