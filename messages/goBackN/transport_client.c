#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define _GNU_SOURCE

#include "messages.h"

#include "transport.h"
#include "transport_client.h"

#include "window.h"

static int id_rts_ots;
static int id_app_dati;
static int id_dati;
static int id_ack;

#define NUM_SENDERS 2
#define NUM_TO_SEND (NUM_MESSAGGI / 2)

#define NUM_PRODUCERS 2
#define NUM_TO_PRODUCE (NUM_MESSAGGI / 2)

#define SEND_MSG_CHANCE .8f

void transport_client(int id_rtsots, int id_appdati) {
    id_rts_ots = id_rtsots;
    id_app_dati = id_appdati;

    printf("[Trasporto client] In esecuzione\n");

    // ottiene le code per la comunicazione con il server
    get_queues();

    int ret;

    pthread_t id_producers[NUM_PRODUCERS];
    pthread_t id_senders[NUM_SENDERS];
    pthread_t id_receiver_ack;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct window* w = malloc(sizeof(struct window));
    if (!w) {
        perror("[Trasporto client] Errore nella malloc window");
        exit(1);
    }
    init_window(w);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        ret = pthread_create(&id_producers[i], &attr, receiver_and_producer, w);
        if (ret != 0) {
            perror("[Trasporto client] Errore nella pthread_create del ricevitore/produttore");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_SENDERS; i++) {
        ret = pthread_create(&id_senders[i], &attr, sender, w);
        if (ret != 0) {
            perror("[Trasporto client] Errore nella pthread_create del sender");
            exit(1);
        }
    }

    ret = pthread_create(&id_receiver_ack, &attr, receiver_ack, w);
    if (ret != 0) {
        perror("[Trasporto client] Errore nella pthread_create del ricevitore ack");
        exit(1);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(id_producers[i], NULL);
    }
    for (int i = 0; i < NUM_SENDERS; i++) {
        pthread_join(id_senders[i], NULL);
    }
    pthread_join(id_receiver_ack, NULL);

    destroy_window(w);
    free(w);

    pthread_attr_destroy(&attr);

    printf("[Trasporto client] Terminazione\n");
}

void* receiver_and_producer(void* arg) {
    struct window* w = arg;

    srand(gettid());

    for (int i = 0; i < NUM_TO_PRODUCE; i++) {
        // attende la ricezione di un RTS
        int ret;

        struct msg_rts_ots rts;
        ret = msgrcv(id_rts_ots, &rts, SIZE_MSG_RTS_OTS, REQUEST_TO_SEND, 0);
        if (ret < 0) {
            perror("[Trasporto client - producer] Errore nella msgrcv della RTS");
            exit(1);
        }

        printf("[Trasporto client - producer %d] Request to send {%d} ricevuta\n", gettid(), rts.id);

        // invia l'ots, attende il messaggio dall'app e poi lo incapsula in un messaggio trasporto.
        // Quindi aggiunge quest'ultimo alla window
        receive_and_produce(id_rts_ots, id_app_dati, w, rts.id);
    }

    pthread_exit(NULL);
}

void* sender(void* arg) {
    struct window* w = arg;

    printf("[Trasporto client - sender %d] Sender avviato con id_dati = %d\n", gettid(), id_dati);

    // manda NUM_TO_SEND messaggi presenti nella window (attendendo che il receiver/producer li produca)
    for (int i = 0; i < NUM_TO_SEND; i++) {
        send(id_dati, SEND_MSG_CHANCE, w);
    }

    pthread_exit(NULL);
}

void* receiver_ack(void* arg) {
    struct window* w = arg;

    receive_ack(id_ack, id_dati, SEND_MSG_CHANCE, w);

    pthread_exit(NULL);
}

void get_queues() {
    // ottiene la coda per l'invio dei dati (messaggi tra i livelli trasporto (client-server))
    key_t key_dati = ftok(".", 'd');
    id_dati = msgget(key_dati, 0);
    if (id_dati < 0) {
        perror("Errore nella msgget dei dati trasporto client");
        exit(1);
    }

    // ottiene la coda per l'invio degli ack (messaggi tra i livelli trasporto (server-client))
    key_t key_ack = ftok(".", 'a');
    id_ack = msgget(key_ack, 0);
    if (id_ack < 0) {
        perror("Errore nella msgget dell'ack trasporto client");
        exit(1);
    }

    printf("[Trasporto client] id_dati = %d, id_ack = %d\n", id_dati, id_ack);
}