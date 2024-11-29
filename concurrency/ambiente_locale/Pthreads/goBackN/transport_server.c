#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messages.h"

#include "transport.h"
#include "transport_server.h"

#define SEND_ACK_CHANCE .8f

void transport_server(int id_app_dati) {
    int id_dati;
    int id_ack;

    printf("[Trasporto server] In esecuzione\n");

    // ottiene le code per la comunicazione con il client
    get_queues(&id_dati, &id_ack);

    int num_seq_atteso = 0;
    while (num_seq_atteso < NUM_MESSAGGI) {
        int ret;

        printf("[Trasporto server] Attendo pacchetto con numero di sequenza %d...\n", num_seq_atteso);
        // si mette in attesa di un messaggio dal livello trasporto client
        struct msg_dati dati;
        ret = msgrcv(id_dati, &dati, SIZE_MSG_DATI, 0, 0);
        if (ret < 0) {
            perror("[Trasporto server] Errore nella msgrcv dei dati");
            exit(1);
        }

        // verifica se il numero di sequenza coincide con quello atteso
        // se non coincide, scarta il pacchetto, altrimenti va avanti
        if (dati.num_sequence != num_seq_atteso) {
            printf("[Trasporto server] Ricevuto un pacchetto non atteso (num_seq = %d, num_seq_atteso = %d). Lo scarto\n",
                   dati.num_sequence,
                   num_seq_atteso);
            continue;
        }

        // incrementa il numero di sequenza atteso
        num_seq_atteso++;

        // invia un messaggio di ack con num_ack = num_seq_atteso (già incrementato) al livello trasporto client
        printf("[Trasporto server] Ricevuto il pacchetto [%d]\n", dati.num_sequence);
        struct msg_ack ack = {
            .type = 1,
            .num_ack = num_seq_atteso,
            .id = rand() % 1000
        };

        ret = send_maybe(id_ack, &ack, SIZE_MSG_ACK, 0, SEND_ACK_CHANCE);
        if (ret) {
            printf("[Trasporto server] L'ack [num ack = %d] {id = %d} è stato inviato con successo!\n", num_seq_atteso, ack.id);
        } else {
            printf("[Trasporto server] L'ack [num ack = %d] {id = %d} SI È PERSO!\n", num_seq_atteso, ack.id);
        }

        // invia il messaggio applicativo all'app server, incapsulato nel messaggio ricevuto
        printf("[Trasporto server] Invio il pacchetto dati estratto dal pacchetto [%d] all'app server\n", dati.num_sequence);
        ret = msgsnd(id_app_dati, &dati.msg, SIZE_MSG_APP, 0);
        if (ret < 0) {
            perror("[Trasporto server] Errore nella msgsnd dei dati all'app");
            exit(1);
        }
    }

    printf("[Trasporto server] Terminazione\n");
}

void get_queues(int* id_dati, int* id_ack) {
    // ottiene la coda per l'invio dei dati (messaggi tra i livelli trasporto (server-server))
    key_t key_dati = ftok(".", 'd');
    *id_dati = msgget(key_dati, 0);
    if (*id_dati < 0) {
        perror("Errore nella msgget dei dati trasporto server");
        exit(1);
    }

    // ottiene la coda per l'invio degli ack (messaggi tra i livelli trasporto (server-server))
    key_t key_ack = ftok(".", 'a');
    *id_ack = msgget(key_ack, 0);
    if (*id_ack < 0) {
        perror("Errore nella msgget dell'ack trasporto server");
        exit(1);
    }
}