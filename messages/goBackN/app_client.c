#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "app_msg.h"

static int id_app_dati;
static int id_app_ack;

const char* frase = "GIOVANNI";
const int lunghezza = 8;

void init() {
    // alloca una coda per l'invio dei messaggi app al livello trasporto del client
    key_t key_app_dati = ftok(".", 'c');
    id_app_dati = msgget(key_app_dati, IPC_CREAT | 0644);
    if (id_app_dati < 0) {
        perror("Errore nella msgget dei dati dell'app verso il trasporto del client");
        exit(1);
    }

    // alloca una coda per la ricezione del messaggio di ACK finale dal livello trasporto del client
    key_t key_app_ack = ftok(".", 'C');
    id_app_ack = msgget(key_app_ack, IPC_CREAT | 0644);
    if (id_app_ack < 0) {
        perror("Errore nella msgget dell'ack trasporto-app del client");
        exit(1);
    }

    // crea il processo del livello trasporto
    pid_t pid = fork();
    if (pid < 0) {
        perror("Errore nella creazione del server trasporto");
        exit(1);
    } else if (pid == 0) {
        execl("./transport_client", "transport_client", NULL);
        perror("Errore nell'execl trasporto client");
        exit(1);
    }
}

void destroy() {
    // dealloca le code
    int ret = msgctl(id_app_dati, IPC_RMID, NULL);
    if (ret < 0) {
        perror("Errore nella deallocazione della coda dei dati app-trasporto del server");
    }
    ret = msgctl(id_app_ack, IPC_RMID, NULL);
    if (ret < 0) {
        perror("Errore nella deallocazione della coda dell'ack trasporto-app del server");
    }
}

int main() {
    printf("[App Client] In esecuzione\n");

    init();

    for (int i = 0; i < lunghezza; i++) {
        // invia i messaggi tramite il livello trasporto

        struct app_msg msg = {
            .type = 1,
            .character = frase[i]
        };

        // invia i messaggi in maniera asincrona tramite il livello trasporto,
        // che si occuperà di garantirne
        // send_message
    }

    destroy();

    printf("[App Client] Terminazione\n");

    return 0;
}