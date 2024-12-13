#ifndef MESSAGGI_H
#define MESSAGGI_H

struct msg_richiesta {
    long type;
};

typedef int buffer_t;

#define NUM_ITERAZIONI 5

#define SIZEOF_MSG(x) (sizeof(x) - sizeof(long))

#endif