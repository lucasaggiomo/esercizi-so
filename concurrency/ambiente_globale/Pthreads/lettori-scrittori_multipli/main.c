#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "wrapper.h"

#include "bufferLS.h"

extern pid_t gettid();

#define NUM_BUFFER 3

#define NUM_MID 2
#define NUM_LEAF_PER_MID 2

#define NUM_SCRITTURE_ROOT 3
#define NUM_SCRITTURE_LETTURE_MID 5
#define NUM_LETTURE_LEAF 10

void* root(void* arg);
void* mid(void* arg);
void* leaf(void* arg);

struct mid_args {
    struct bufferLS* read_buffer;      // buffer da cui leggere
    struct bufferLS* write_buffer;     // buffer su cui scrivere
};

int main() {
    pthread_t id_root;
    pthread_t id_mids[NUM_MID];
    pthread_t id_leafs[NUM_MID * NUM_LEAF_PER_MID];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct bufferLS* buffers = Malloc(NUM_BUFFER * sizeof(struct bufferLS));
    for (int i = 0; i < NUM_BUFFER; i++) {
        init_buffer(&buffers[i]);
    }

    Pthread_create(&id_root, &attr, root, &buffers[0]);

    struct mid_args* all_mid_args[NUM_MID];

    for (int i = 0; i < NUM_MID; i++) {
        all_mid_args[i] = Malloc(sizeof(struct mid_args));
        all_mid_args[i]->read_buffer = &buffers[0];
        all_mid_args[i]->write_buffer = &buffers[i + 1];

        Pthread_create(&id_mids[i], &attr, mid, all_mid_args[i]);

        for (int j = 0; j < NUM_LEAF_PER_MID; j++) {
            Pthread_create(&id_leafs[i * NUM_LEAF_PER_MID + j], &attr, leaf, all_mid_args[i]->write_buffer);
        }
    }

    Pthread_join(id_root, NULL);

    for (int i = 0; i < NUM_MID; i++) {
        Pthread_join(id_mids[i], NULL);

        for (int j = 0; j < NUM_LEAF_PER_MID; j++) {
            Pthread_join(id_leafs[i * NUM_LEAF_PER_MID + j], NULL);
        }

        free(all_mid_args[i]);
    }
    for (int i = 0; i < NUM_BUFFER; i++) {
        destroy_buffer(&buffers[i]);
    }
    free(buffers);

    pthread_attr_destroy(&attr);

    return 0;
}

void* root(void* arg) {
    struct bufferLS* b = arg;

    srand(gettid());

    for (int i = 0; i < NUM_SCRITTURE_ROOT; i++) {
        // scrive sul buffer un valore a caso da 0 a 9
        int val = rand() % 10;

        scrivi(b, val);
        printf("[Root %d] Ho scritto %d\n", gettid(), val);

        sleep(3);
    }

    pthread_exit(NULL);
}

void* mid(void* arg) {
    struct mid_args* a = arg;

    sleep(2);

    for (int i = 0; i < NUM_SCRITTURE_LETTURE_MID; i++) {
        int val = leggi(a->read_buffer);

        printf("[Mid %d] Ho letto %d\n", gettid(), val);

        scrivi(a->write_buffer, val);

        printf("[Mid %d] Ho scritto %d\n", gettid(), val);

        sleep(2);
    }

    pthread_exit(NULL);
}

void* leaf(void* arg) {
    struct bufferLS* b = arg;

    sleep(5);

    for (int i = 0; i < NUM_SCRITTURE_LETTURE_MID; i++) {
        int val = leggi(b);

        printf("[Leaf %d] Ho letto %d\n", gettid(), val);

        sleep(1);
    }

    pthread_exit(NULL);
}