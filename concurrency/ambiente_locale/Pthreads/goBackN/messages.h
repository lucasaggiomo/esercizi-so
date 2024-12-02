#ifndef MESSAGES_H
#define MESSAGES_H

// struttura per i messaggi tra i livelli applicativi
struct msg_app {
    long type;
    char character;
};

// messaggi per la sincronizzazione
struct msg_rts_ots {
    long type;
    int id;     // random number to identify message (for debugging purposes)
};

#define REQUEST_TO_SEND 1
#define OK_TO_SEND 2

#define SIZE_MSG_APP (sizeof(struct msg_app) - sizeof(long))
#define SIZE_MSG_RTS_OTS (sizeof(struct msg_rts_ots) - sizeof(long))

#define NUM_MESSAGGI 8

#endif