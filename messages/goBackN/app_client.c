#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "app_client.h"

const char* frase = "GIOVANNI";
const int lunghezza = 8;

void send_sincrona(int id_rts_ots, int id_app_dati, struct msg_app* msg) {
    int ret;

    // send asincrona RTS
    struct msg_rts_ots rts = {
        .type = REQUEST_TO_SEND,
        .id = rand() % 10000
    };
    ret = msgsnd(id_rts_ots, &rts, SIZE_MSG_RTS_OTS, 0);
    if (ret < 0) {
        perror("[App Client] Errore msgsnd RTS");
        exit(1);
    }
    printf("[App Client] Request to send {%d} inviata\n", rts.id);

    // receive bloccante OTS
    struct msg_rts_ots ots;
    ret = msgrcv(id_rts_ots, &ots, SIZE_MSG_RTS_OTS, OK_TO_SEND, 0);
    if (ret < 0) {
        perror("[App Client] Errore msgrcv OTS");
        exit(1);
    }

    printf("[App Client] Ok to send {%d} ricevuta\n", ots.id);

    // send asincrona messaggio
    ret = msgsnd(id_app_dati, msg, SIZE_MSG_APP, 0);
    if (ret < 0) {
        perror("[App Client] Errore msgsnd app -> trasporto");
        exit(1);
    }

    printf("[App Client] Messaggio \'%c\' inviato\n", msg->character);
}

void app_client(int id_rts_ots, int id_app_dati) {
    printf("[App Client] In esecuzione\n");

    for (int i = 0; i < lunghezza; i++) {
        // invia i messaggi tramite il livello trasporto in maniera sincrona
        // ovvero manda una REQUEST TO SEND, poi attende una OK TO SEND
        // e infine manda il messaggio effettivo
        struct msg_app msg = {
            .type = 1,
            .character = frase[i]
        };
        send_sincrona(id_rts_ots, id_app_dati, &msg);

        sleep(5);
    }

    // termina
    printf("[App Client] Terminazione\n");
}