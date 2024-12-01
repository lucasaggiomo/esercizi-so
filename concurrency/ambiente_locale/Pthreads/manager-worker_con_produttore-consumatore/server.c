#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "shared.h"
#include "wrapper.h"

extern int gettid();

#define NUM_WORKERS 2

static int id_richieste;
static int id_risposte;
static pthread_t id_workers[NUM_WORKERS];

#define MAX_RICHIESTE 2

void* manager(void* arg);
void* worker(void* arg);

int main() {
    key_t key_richeste = ftok(".", 'q');
    id_richieste = Msgget(key_richeste, 0);

    key_t key_risposte = ftok(".", 'a');
    id_risposte = Msgget(key_risposte, 0);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_manager;

    struct shared* sh = Malloc(sizeof(*sh));
    init_shared(sh, MAX_RICHIESTE);

    Pthread_create(&id_manager, &attr, manager, sh);

    for (int i = 0; i < NUM_WORKERS; i++) {
        Pthread_create(&id_workers[i], &attr, worker, sh);
    }

    Pthread_join(id_manager, NULL);

    destroy_shared(sh);

    free(sh);

    pthread_attr_destroy(&attr);

    return 0;
}

void* manager(void* arg) {
    struct shared* sh = arg;

    while (1) {
        struct msg_richiesta richiesta;
        Msgrcv(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0, 0);

        if (richiesta.values[0] == -1 && richiesta.values[1] == -1) {
            printf("[Manager %d] Ricevuto una richiesta di terminazione\n", gettid());
            break;
        }

        printf("[Manager %d] Ricevuto una richiesta da %d contenente [%d, %d]. Lo aggiungo al buffer condiviso\n",
               gettid(),
               richiesta.pid,
               richiesta.values[0], richiesta.values[1]);

        produci(sh, &richiesta);

        sleep(1);
    }

    for (int i = 0; i < NUM_WORKERS; i++) {
        Pthread_cancel(id_workers[i]);
    }
    pthread_exit(NULL);
}

void* worker(void* arg) {
    struct shared* sh = arg;

    while (1) {
        struct msg_richiesta richiesta;
        consuma(sh, &richiesta);

        printf("[Worker %d] Consumato una richiesta da %d contenente [%d, %d]\n",
               gettid(),
               richiesta.pid,
               richiesta.values[0], richiesta.values[1]);

        struct msg_risposta risposta = {
            .type = richiesta.pid,
            .result = richiesta.values[0] * richiesta.values[1]
        };

        printf("[Worker %d] Invio risposta con %d\n", gettid(), risposta.result);
        Msgsnd(id_risposte, &risposta, SIZE_MSG_RISPOSTA, 0);
    }
}