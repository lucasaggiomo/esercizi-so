#ifndef MESSAGGI_H
#define MESSAGGI_H

struct msg_richiesta {
    long type;
    int quantita;
};

#define SIZEOF_MSG(msg) (sizeof(msg) - sizeof(long))

#endif