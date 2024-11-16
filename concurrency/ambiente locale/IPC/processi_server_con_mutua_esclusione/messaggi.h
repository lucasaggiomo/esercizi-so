#ifndef _HEADER_H
#define _HEADER_H

#include "data.h"

struct msg {
    long type;
    struct data data;
};

#define SIZE_MSG (sizeof(struct msg) - sizeof(long))

#endif