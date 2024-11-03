#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 2

struct counter {
    pthread_mutex_t mutex;
    int valore;
};

void* counter(void* x) {
    struct counter* p = x;
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&p->mutex);

        p->valore++;

        pthread_mutex_unlock(&p->mutex);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];

    // creo l'area di memoria nell'heap (quindi è condivisa a tutti i thread del processo)
    struct counter* p = malloc(sizeof(*p));
    if (!p) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    p->valore = 0;
    pthread_mutex_init(&p->mutex, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], &attr, counter, p);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Valore del contatore = %d\n", p->valore);

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&p->mutex);

    free(p);
}