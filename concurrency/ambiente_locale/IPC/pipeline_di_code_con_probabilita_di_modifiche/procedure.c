#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"

#include "procedure.h"

void client(int id_iniettore, const char* str, int num_messaggi) {
    struct msg msg = {
        .type = 1
    };

    srand(getpid());

    sleep(rand() % 3 + 1);

    for (int i = 1; i <= num_messaggi; i++) {
        msg.value = snprintf(msg.str, DIM_STRING, "%s_%d", str, i);

        printf("[Client %d] Invio un messaggio [%s, %ld]\n", getpid(), msg.str, msg.value);

        int ret = msgsnd(id_iniettore, &msg, SIZE_MSG, 0);
        if (ret < 0) {
            perror("Errore nella msgsnd");
            exit(1);
        }

        sleep(1);
    }
}

void iniettore(int id_client, int id_server, float value_corruption_chance, float str_corruption_chance, int num_messaggi) {
    struct msg msg_from_client;
    int ret;

    srand(getpid());

    for (int i = 0; i < num_messaggi; i++) {
        ret = msgrcv(id_client, &msg_from_client, SIZE_MSG, 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        printf("[Iniettore] Ho ricevuto il messaggio [%s, %ld]\n", msg_from_client.str, msg_from_client.value);

        float chance = (float)(rand() % 1000001) / 1000000;     // random float da 0.0f a 1.0f con risoluzione 1e-6
        if (chance < value_corruption_chance) {
            printf("[Iniettore] CORROMPO IL NUMERO INTERO AHAHHAHAHHA\n");
            msg_from_client.value += rand();
        } else if (chance + value_corruption_chance < str_corruption_chance) {
            printf("[Iniettore] CORROMPO LA STRINGA AHAHHAHAHHA\n");
            int rand_index = rand() % DIM_STRING;
            msg_from_client.str[rand_index] = '\0';
        } else {
            printf("[Iniettore] Questa volta sono stato buono...\n");
        }

        struct msg msg_to_server;
        memcpy(&msg_to_server, &msg_from_client, sizeof(msg_from_client));
        printf("[Iniettore] Mando il messaggio [%s, %ld]\n", msg_to_server.str, msg_to_server.value);

        ret = msgsnd(id_server, &msg_to_server, SIZE_MSG, 0);
        if (ret < 0) {
            perror("Errore nella msgsnd");
            exit(1);
        }
    }
}

void server(int id_iniettore, int num_messaggi) {
    struct msg msg;
    int ret;

    srand(getpid());

    for (int i = 0; i < num_messaggi; i++) {
        ret = msgrcv(id_iniettore, &msg, SIZE_MSG, 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        if (strlen(msg.str) == msg.value) {
            printf("[Server] Ho ricevuto il messaggio [%s, %ld] che non sembra corrotto\n", msg.str, msg.value);
        } else {
            printf("[Server] Ho ricevuto il messaggio [%s, %ld] che è SICURAMENTE CORROTTO\n", msg.str, msg.value);
        }
    }
}