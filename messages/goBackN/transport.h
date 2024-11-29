#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "messages.h"

// messaggi dati tra i livelli trasporto
// incapsulano un messaggio applicativo
struct msg_dati {
    long type;
    int num_sequence;     // numero progressivo del messaggio dati
    struct msg_app msg;
};

// messaggi di responso (ack) tra i livelli trasporto
struct msg_ack {
    long type;
    int num_ack;     // numero progressivo dell'ack (secondo la logica dell'algoritmo)
    int id;          // random number for debugging purposes
};

#define SIZE_MSG_DATI (sizeof(struct msg_dati) - sizeof(long))
#define SIZE_MSG_ACK (sizeof(struct msg_ack) - sizeof(long))

#define WINDOW_SIZE 3

#define TIMEOUT 5

#define MAX_SEQ __INT_MAX__

// manda il messaggio con una certa probabilità, altrimenti lo scarta
// chance deve essere un numero da 0.0f a 1.0f
// restituisce 1 se il messaggio è stato mandato, 0 altrimenti
int send_maybe(int id_coda, void* msg, size_t size_msg, int msgflg, float chance);

// restutuisce 1 con una certa probabilità data da chance, altrimenti restituisce 0
// chance deve essere un numero da 0.0f a 1.0f
int toDoOrNotToDo(float chance);

#endif