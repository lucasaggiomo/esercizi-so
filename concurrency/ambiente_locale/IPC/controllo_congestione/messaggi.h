#ifndef MESSAGGI_H
#define MESSAGGI_H

struct msg_data {
    long type;
    int data;
};

struct msg_token {
    long type;
};

#define SIZE_MSG_DATA (sizeof(struct msg_data) - sizeof(long))
#define SIZE_MSG_TOKEN (sizeof(struct msg_token) - sizeof(long))

#endif