#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#define NUM_THREADS 2
#define NUM_MESSAGGI 6

struct param {
    int id_rts;
    int id_ots;
    int id_data;
};

extern int gettid();

void* thread_routine(void* arg);
void receive(int id_rts, int id_ots, int id_data, struct msg_data* msg);

int main() {
    key_t key_rts = ftok(".", 'r');
    int id_rts = Msgget(key_rts, 0);

    key_t key_ots = ftok(".", 'o');
    int id_ots = Msgget(key_ots, 0);

    key_t key_data = ftok(".", 'd');
    int id_data = Msgget(key_data, 0);

    // crea due thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        struct param* p = malloc(sizeof(*p));
        p->id_data = id_data;
        p->id_ots = id_ots;
        p->id_rts = id_rts;
        Pthread_create(&id_threads[i], &attr, thread_routine, p);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        Pthread_join(id_threads[i], NULL);
    }

    pthread_attr_destroy(&attr);
}

void* thread_routine(void* arg) {
    struct param* p = arg;

    struct msg_data msg;

    for (int i = 0; i < NUM_MESSAGGI; i++) {
        receive(p->id_rts, p->id_ots, p->id_data, &msg);

        int sum = msg.values[0] + msg.values[1];
        printf("[Server %d] Ho ricevuto [%d, %d] e la somma è %d\n", gettid(), msg.values[0], msg.values[1], sum);
    }

    free(arg);

    pthread_exit(NULL);
}

void receive(int id_rts, int id_ots, int id_data, struct msg_data* msg) {
    printf("[Server %d] Attendo REQUEST TO SEND...\n", gettid());

    // attende una rts
    struct msg_rts rts;
    Msgrcv(id_rts, &rts, SIZE_MSG_RTS, 0, 0);

    printf("[Server %d] Invio OK TO SEND\n", gettid());

    // invia una ots
    struct msg_ots ots = {
        .type = rts.pid
    };
    Msgsnd(id_ots, &ots, SIZE_MSG_OTS, 0);

    printf("[Server %d] Attendo un messaggio...\n", gettid());

    // attende un messaggio
    Msgrcv(id_data, msg, SIZE_MSG_DATA, rts.pid, 0);
}