#ifndef MESSAGGI_H
#define MESSAGGI_H

#define DIM_STRING 21

#include <sys/types.h>

struct msg {
    long type;
    size_t value;
    char str[21];
};

#define SIZE_MSG (sizeof(struct msg) - sizeof(long))

#endif