#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#include "procedure.h"

void produci(int id_data, int id_token) {
    // printf("[Produttore %d] Attendo un token...\n", getpid());

    // attende la presenza di un token
    struct msg_token token;
    Msgrcv(id_token, &token, SIZE_MSG_TOKEN, 0, 0);

    // produce il messaggio
    struct msg_data data = {
        .type = 1,
        .data = rand() % 20
    };
    Msgsnd(id_data, &data, SIZE_MSG_DATA, 0);

    printf("[Produttore %d] Ho prodotto %d\n", getpid(), data.data);
}
void consuma(int id_data, int id_token) {
    // printf("[Consumatore %d] Attendo un messaggio...\n", getpid());

    // attende la presenza di un messaggio
    struct msg_data data;
    Msgrcv(id_data, &data, SIZE_MSG_DATA, 0, 0);

    printf("[Consumatore %d] Ho letto %d\n", getpid(), data.data);

    // produce un token
    struct msg_token token = {
        .type = 1,
    };
    Msgsnd(id_token, &token, SIZE_MSG_TOKEN, 0);

    printf("[Consumatore %d] Ho prodotto un token\n", getpid());
}