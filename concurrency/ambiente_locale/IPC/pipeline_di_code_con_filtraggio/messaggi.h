#ifndef MESSAGGI_H
#define MESSAGGI_H

struct msg_generato {
    long type;
    char str[10];
    int values[2];
    long result;
};

#define SIZE_MSG_DATA (sizeof(struct msg_generato) - sizeof(long))

#endif