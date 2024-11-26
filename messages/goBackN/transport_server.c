#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "app_msg.h"
#include "msg_ack.h"
#include "msg_dati.h"

int main() {
    printf("[Trasporto server] In esecuzione\n");

    // ottiene la coda per l'invio dei dati (messaggi tra i livelli trasporto (client-server))
    key_t key_dati = ftok(".", 'd');
    int id_dati = msgget(key_dati, 0);
    if (id_dati < 0) {
        perror("Errore nella msgget dei dati");
        exit(1);
    }

    // ottiene la coda per l'invio degli ack (messaggi tra i livelli trasporto (server-client))
    key_t key_ack = ftok(".", 'a');
    int id_ack = msgget(key_ack, 0);
    if (id_ack < 0) {
        perror("Errore nella msgget dell'ack");
        exit(1);
    }

    // ottiene la coda per l'invio dei messaggi app al livello trasporto del server
    key_t key_app_dati = ftok(".", 's');
    int id_app_dati = msgget(key_app_dati, 0);
    if (id_app_dati < 0) {
        perror("Errore nella msgget dei dati dell'app verso il trasporto del server");
        exit(1);
    }

    // ottiene la coda per la ricezione del messaggio di ACK finale dal livello trasporto
    key_t key_app_ack = ftok(".", 'S');
    int id_app_ack = msgget(key_app_ack, 0);
    if (id_app_ack < 0) {
        perror("Errore nella msgget dell'ack trasporto-app del server");
        exit(1);
    }

    printf("[Trasporto server] Terminazione\n");

    return 0;
}