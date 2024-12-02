#ifndef WINDOW_H
#define WINDOW_H

#include "transport.h"     // solo per window size (mi scoccio di fare la window nell'heap)

#include <pthread.h>

extern int gettid();

enum status {
    FREE,
    IN_USE,
    NOT_YET_SENT,
    WAITING_FOR_ACK
};

struct window {
    struct msg_dati buffer[WINDOW_SIZE];     // Pacchetti nella finestra
    enum status status[WINDOW_SIZE];         // Stato degli elementi del buffer
    int send_base;                           // Numero di sequenza del messaggio più vecchio inviato (coda = send_base % WINDOW_SIZE)
    int next_seq_num;                        // Numero di sequenza del prossimo messaggio da inviare (testa = next_seq_num % WINDOW_SIZE)
    int count;                               // Numero di messaggi contenuti nella window (di cui si attende l'ACK)

    // variabili per la gestione della concorrenza
    int num_free;                // Numero di elementi free della window
    int num_to_send;             // Numero di elementi nella window da mandare
    int num_waiting_for_ack;     // Numero di messaggi inviati per cui si attende l'ack
    pthread_mutex_t mutex;       // Mutex per la mutua esclusione
    pthread_cond_t cv_producer;
    pthread_cond_t cv_sender;
    pthread_cond_t cv_consumer;
};

void init_window(struct window* w);
void destroy_window(struct window* w);

void read_window(struct window* w);

// invia l'ots all'app, riceve il messaggio e produce nella window
void receive_and_produce(int id_rts_ots, int id_app_dati, struct window* w, int ots_id);

// invia un messaggio dalla window
void send(int id_dati, float chance_to_send, struct window* w);
void receive_ack(int id_ack, int id_dati, float chance_to_send, struct window* w);

#endif