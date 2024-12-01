#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define _GNU_SOURCE

#include "shared.h"
#include "wrapper.h"

#define NUM_LETTORI 3
#define NUM_LETTURE 5

#define NUM_SCRITTORI 2
#define NUM_SCRITTURE 10

extern pid_t gettid();

void* lettore(void* arg);
void* scrittore(void* arg);

int main() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_lettori[NUM_LETTORI];
    pthread_t id_scrittori[NUM_SCRITTORI];

    struct shared* sh = Malloc(sizeof(*sh));
    init_shared(sh);

    for (int i = 0; i < NUM_LETTORI; i++) {
        Pthread_create(&id_lettori[i], &attr, lettore, sh);
    }
    for (int i = 0; i < NUM_SCRITTORI; i++) {
        Pthread_create(&id_scrittori[i], &attr, scrittore, sh);
    }

    for (int i = 0; i < NUM_LETTORI; i++) {
        Pthread_join(id_lettori[i], NULL);
    }

    for (int i = 0; i < NUM_SCRITTORI; i++) {
        Pthread_join(id_scrittori[i], NULL);
    }

    destroy_shared(sh);
    free(sh);

    pthread_attr_destroy(&attr);

    return 0;
}

void* lettore(void* arg) {
    struct shared* sh = arg;

    srand(gettid());

    sleep(rand() % 3 + 1);

    int versione = 0;
    int data;
    for (int i = 0; i < NUM_LETTURE; i++) {
        data = leggi(sh, &versione);

        printf("[Lettore %d] Ho letto %d, versione %d\n", gettid(), data, versione);
    }

    pthread_exit(NULL);
}

void* scrittore(void* arg) {
    struct shared* sh = arg;

    srand(gettid());

    int versione = 0;
    int data;
    for (int i = 0; i < NUM_SCRITTURE; i++) {
        data = rand() % 100;

        versione = scrivi(sh, data);

        printf("[Scrittore %d] Ho scritto %d, versione %d\n", gettid(), data, versione);

        sleep(1);
    }

    pthread_exit(NULL);
}