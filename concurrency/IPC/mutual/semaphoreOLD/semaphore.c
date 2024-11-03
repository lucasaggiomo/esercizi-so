#define _GNU_SOURCE
#include "semaphore.h"
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "queue.h"

void init_semaphore(struct semaphore* s, int queueCapacity, int startingValue) {
    if (!s)
        return;

    s->value = startingValue;

    struct queue Q;
    init_queue(&Q, queueCapacity);
}
void destroy_semaphore(struct semaphore* s) {
    if (!s)
        return;

    destroy_queue(&s->queue);
}

void wait_semaphore(struct semaphore* s, int tid) {
    s->value--;
    if (s->value < 0) {
        enqueue(&s->queue, tid);  // entra nella coda del semaforo
        kill(tid, SIGSTOP);       // si sospende
    }
}
void signal_semaphore(struct semaphore* s) {
    s->value++;
    if (s->value <= 0) {
        int tid = dequeue(&s->queue);  // preleva dalla coda
        kill(tid, SIGCONT);            // sveglia il processo tid (successivo nel semaforo)
        // PROBLEMA: kill manda il segnale a tutto il processo, quindi a tutti i thread
    }
}