#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "wrapper.h"

#include "buffer_m.h"
#include "vettore_m.h"

extern pid_t gettid();

#define DIM_BUFFER 5

#define NUM_DESTINATARI 3
#define NUM_CONSULTI 6

#define NUM_GENERAZIONI 10

void* generatore(void* arg);
void* aggiornatore(void* arg);
void* destinatario(void* arg);

int main() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_generatore;
    pthread_t id_aggiornatore;
    pthread_t id_destinatari[NUM_DESTINATARI];

    struct vettore_m* v = Malloc(sizeof(struct vettore_m));
    init_vettore(v, DIM_BUFFER);

    struct buffer_m* b = Malloc(sizeof(struct buffer_m));
    init_buffer(b);

    void** v_and_b = Malloc(2 * sizeof(void*));
    v_and_b[0] = v;
    v_and_b[1] = b;

    Pthread_create(&id_generatore, &attr, generatore, v);
    Pthread_create(&id_aggiornatore, &attr, aggiornatore, v_and_b);
    for (int i = 0; i < NUM_DESTINATARI; i++) {
        Pthread_create(&id_destinatari[i], &attr, destinatario, b);
    }

    Pthread_join(id_generatore, NULL);
    Pthread_join(id_aggiornatore, NULL);
    for (int i = 0; i < NUM_DESTINATARI; i++) {
        Pthread_join(id_destinatari[i], NULL);
    }

    destroy_buffer(b);
    destroy_vettore(v);

    free(v);
    free(b);
    free(v_and_b);

    pthread_attr_destroy(&attr);

    return 0;
}

void* generatore(void* arg) {
    struct vettore_m* v = arg;

    srand(gettid());

    for (int i = 0; i < NUM_GENERAZIONI; i++) {
        struct item e = {
            .a = rand() % 11,
            .b = rand() % 11
        };

        printf("[Generatore] Genero l'elemento [%d, %d]\n", e.a, e.b);

        genera(v, e);
    }

    pthread_exit(NULL);
}

void* aggiornatore(void* arg) {
    struct vettore_m* v = ((void**)arg)[0];
    struct buffer_m* b = ((void**)arg)[1];

    for (int i = 0; i < NUM_GENERAZIONI; i++) {
        struct item e = preleva(v);

        printf("[Aggiorntore] Ho prelevato [%d, %d]\n", e.a, e.b);

        aggiorna(b, e);

        printf("[Aggiorntore] Ho aggiornato [%d, %d]\n", e.a, e.b);

        sleep(1);
    }

    pthread_exit(NULL);
}

void* destinatario(void* arg) {
    struct buffer_m* b = arg;

    srand(gettid());

    for (int i = 0; i < NUM_CONSULTI; i++) {
        sleep(2);

        struct item e = consulta(b);

        printf("[Destinatario %d] Ho consultato [%d, %d]\n", gettid(), e.a, e.b);
    }

    pthread_exit(NULL);
}