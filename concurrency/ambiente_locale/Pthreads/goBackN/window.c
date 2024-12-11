#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define _GNU_SOURCE

#include "window.h"

#include "messages.h"

void init_window(struct window* w) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        w->status[i] = FREE;
    }
    w->send_base = 0;
    w->next_seq_num = 0;
    w->count = 0;

    w->num_free = WINDOW_SIZE;
    w->num_to_send = 0;
    w->num_waiting_for_ack = 0;

    pthread_mutex_init(&w->mutex, NULL);
    pthread_cond_init(&w->cv_producer, NULL);
    pthread_cond_init(&w->cv_sender, NULL);
    pthread_cond_init(&w->cv_consumer, NULL);

    read_window(w);
}

void destroy_window(struct window* w) {
    pthread_mutex_destroy(&w->mutex);
    pthread_cond_destroy(&w->cv_producer);
    pthread_cond_destroy(&w->cv_sender);
    pthread_cond_destroy(&w->cv_consumer);
}

void read_window(struct window* w) {
    printf("\n[ ");
    for (int i = 0; i < WINDOW_SIZE; i++) {
        printf("\'%c\'", w->buffer[i].msg.character);
        if (i != WINDOW_SIZE - 1)
            printf("\t");
    }
    printf(" ]\n");
    printf("[ ");
    for (int i = 0; i < WINDOW_SIZE; i++) {
        switch (w->status[i]) {
            case FREE:
                printf("FREE");
                break;
            case NOT_YET_SENT:
                printf("NYS");
                break;
            case WAITING_FOR_ACK:
                printf("ACK");
                break;
            case IN_USE:
                printf("USED");
                break;
        }
        if (i != WINDOW_SIZE - 1)
            printf("\t");
    }
    printf(" ]\n");
    printf("send_base = %d, next_seq_number = %d, num_free = %d, num_to_send = %d, num_waining_for_ack = %d\n\n", w->send_base, w->next_seq_num, w->num_free, w->num_to_send, w->num_waiting_for_ack);
}

void receive_and_produce(int id_rts_ots, int id_app_dati, struct window* w, int ots_id) {
    int ret;
    int index;
    int num_seq;

    // attende che ci sia spazio nella window e ottiene l'indice in cui lavorare
    pthread_mutex_lock(&w->mutex);

    while (w->num_free == 0) {
        pthread_cond_wait(&w->cv_producer, &w->mutex);
    }

    // prende come elemento libero quello puntato da next_seq_num % WINDOW_SIZE (la testa)
    index = w->next_seq_num % WINDOW_SIZE;
    num_seq = w->next_seq_num;
    w->status[index] = IN_USE;
    w->num_free--;
    w->next_seq_num++;

    read_window(w);

    pthread_mutex_unlock(&w->mutex);

    // invia l'ots
    struct msg_rts_ots ots = {
        .type = OK_TO_SEND,
        .id = ots_id
    };
    ret = msgsnd(id_rts_ots, &ots, SIZE_MSG_RTS_OTS, 0);
    if (ret < 0) {
        perror("[Trasporto client - producer] Errore nella msgsnd della OTS");
        exit(1);
    }

    printf("[Trasporto client - producer %d] Ok to send {%d} inviata\n", gettid(), ots.id);

    // attende il messaggio
    struct msg_app msg;
    ret = msgrcv(id_app_dati, &msg, SIZE_MSG_APP, 0, 0);
    if (ret < 0) {
        perror("[Trasporto client - producer] Errore nella msgrcv del messaggio app");
        exit(1);
    }

    printf("[Trasporto client - producer %d] Messaggio applicativo \'%c\' ricevuto\n", gettid(), msg.character);

    // incapsula il messaggio in un messaggio di trasporto e lo inserisce nella window
    struct msg_dati dati = {
        .type = 1,
        .num_sequence = num_seq,
    };
    memcpy(&dati.msg, &msg, sizeof(msg));

    memcpy(&w->buffer[index], &dati, sizeof(dati));

    printf("[Trasporto client - producer %d] Messaggio trasporto [%d] \'%c\' prodotto nell'indice %d della window\n", gettid(), dati.num_sequence, msg.character, index);

    // aggiorna lo stato e segnala i sender
    pthread_mutex_lock(&w->mutex);

    w->status[index] = NOT_YET_SENT;
    w->num_to_send++;

    read_window(w);

    pthread_cond_signal(&w->cv_sender);

    pthread_mutex_unlock(&w->mutex);
}

void send(int id_dati, float chance_to_send, struct window* w) {
    int ret;
    int index;

    // attende che ci sia un messaggio da inviare
    pthread_mutex_lock(&w->mutex);

    while (w->num_to_send == 0) {
        pthread_cond_wait(&w->cv_sender, &w->mutex);
    }

    // prende come elemento libero il primo NOT YET SENT che trova a partire dal send base
    index = w->send_base % WINDOW_SIZE;
    while (w->status[index] != NOT_YET_SENT) {
        index = (index + 1) % WINDOW_SIZE;
    }
    w->status[index] = IN_USE;
    w->num_to_send--;

    read_window(w);

    pthread_mutex_unlock(&w->mutex);

    printf("[Trasporto client - sender %d] Mando il messaggio trasporto [%d] \'%c\' (index = %d in window) al server\n", gettid(), w->buffer[index].num_sequence, w->buffer[index].msg.character, index);

    // invia il messaggio
    ret = send_maybe(id_dati, &w->buffer[index], SIZE_MSG_DATI, 0, chance_to_send);
    if (ret) {
        printf("[Trasporto client - sender %d] Messaggio trasporto [%d] \'%c\' inviato con successo!\n", gettid(), w->buffer[index].num_sequence, w->buffer[index].msg.character);
    } else {
        printf("[Trasporto client - sender %d] Il messaggio trasporto [%d] \'%c\' SI È PERSO!\n", gettid(), w->buffer[index].num_sequence, w->buffer[index].msg.character);
    }

    // aggiorna lo stato e segnala i ricevitori di ack (per far partire il timer)
    pthread_mutex_lock(&w->mutex);

    w->status[index] = WAITING_FOR_ACK;
    w->num_waiting_for_ack++;

    read_window(w);

    // sveglia un ricevitore di ack
    pthread_cond_signal(&w->cv_consumer);

    pthread_mutex_unlock(&w->mutex);
}

void receive_ack(int id_ack, int id_dati, float chance_to_send, struct window* w) {
    int ret;
    struct msg_ack ack;

    while (w->send_base < NUM_MESSAGGI) {
        pthread_mutex_lock(&w->mutex);

        // attende che ci siano messaggi in attesa di ACK
        while (w->num_waiting_for_ack == 0) {
            pthread_cond_wait(&w->cv_consumer, &w->mutex);
        }

        // identifica il pacchetto più vecchio in attesa di ACK
        // int oldest_index = w->send_base % WINDOW_SIZE;
        int oldest_num_seq = w->send_base;

        pthread_mutex_unlock(&w->mutex);

        // simula un timer con il timeout
        printf("[Trasporto client - acker %d] Attendo ack...\n", gettid());

#define RICEVUTO 0
#define NON_RICEVUTO 1

        int stato_ricezione_ack = NON_RICEVUTO;

        for (int i = 0; i < TIMEOUT; i++) {
            sleep(1);
            ret = msgrcv(id_ack, &ack, SIZE_MSG_ACK, 0, IPC_NOWAIT);
            if (ret >= 0) {
                stato_ricezione_ack = RICEVUTO;
                break;
            } else if (errno == ENOMSG) {
                stato_ricezione_ack = NON_RICEVUTO;
            } else {
                perror("[Trasporto client - acker] Errore nella msgrcv dell'ACK");
                exit(1);
            }
        }
        // sleep(TIMEOUT);

        pthread_mutex_lock(&w->mutex);

        switch (stato_ricezione_ack) {
            case RICEVUTO: {     // Caso 1: ACK ricevuto
                if (ack.num_ack <= w->send_base) {
                    // ACK già noto, non fa nulla
                    printf("[Trasporto client - acker %d] Ack ricevuto con numero %d, {id = %d} ma send_base = %d, quindi non faccio nulla\n", gettid(), ack.num_ack, ack.id, w->send_base);
                    pthread_mutex_unlock(&w->mutex);
                    continue;
                }

                printf("[Trasporto client - acker %d] Ack ricevuto con numero %d {id = %d} e send_base = %d, quindi aggiorno la window\n", gettid(), ack.num_ack, ack.id, w->send_base);

                // ACK cumulativo, libera tutti i pacchetti fino a ack.num_ack - 1
                for (int i = w->send_base; i < ack.num_ack; i++) {
                    int idx = i % WINDOW_SIZE;
                    w->status[idx] = FREE;
                    w->num_free++;
                    pthread_cond_signal(&w->cv_producer);     // segnala che c'è spazio per produrre nuovi pacchetti nella window
                }

                // aggiorna il send_base e il numero di waiting for ack
                w->send_base = ack.num_ack;
                w->num_waiting_for_ack -= (ack.num_ack - oldest_num_seq);
            }
            // se ci sono altri pacchetti in attesa di ACK, il ciclo riparte e il "timer" continua
            break;

            case NON_RICEVUTO: {
                printf("[Trasporto client - acker %d] !!TIMEOUT!! Ack non ricevuto, quindi rimando i pacchetti WAITING_FOR_ACK\n", gettid());

                // Caso 2: ACK non ricevuto, timeout, quindi ritrasmetto tutti i pacchetti da send_base a next_seq_num - 1
                // nota: si potrebbe segnalare i sender, in modo da farlo in concorrenza, ma la logica si complicherebbe
                for (int i = w->send_base; i < w->next_seq_num; i++) {
                    int idx = i % WINDOW_SIZE;
                    if (w->status[idx] == WAITING_FOR_ACK) {
                        // invia il pacchetto
                        ret = send_maybe(id_dati, &w->buffer[idx], SIZE_MSG_DATI, 0, chance_to_send);
                        if (ret) {
                            printf("[Trasporto client - acker %d] Messaggio trasporto [%d] \'%c\' ritrasmesso con successo!\n", gettid(), w->buffer[idx].num_sequence, w->buffer[idx].msg.character);
                        } else {
                            printf("[Trasporto client - acker %d] Il messaggio trasporto [%d] \'%c\' ritrasmesso SI È PERSO!\n", gettid(), w->buffer[idx].num_sequence, w->buffer[idx].msg.character);
                        }
                    }
                }
            } break;
        }

        read_window(w);

        pthread_mutex_unlock(&w->mutex);
    }
}