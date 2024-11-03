#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif

#define gettid() ((pid_t)syscall(SYS_gettid))

#define NUM_OF_THREADS 10

void* func(void* arg) {
    printf("[pid: %d, tid: %d] Ho iniziato l'esecuzione di func\n", getpid(), gettid());
    sleep(1);

    int* exit_value = malloc(sizeof(int));
    if (!exit_value)
        pthread_exit(NULL);

    *exit_value = 0;
    printf("[pid: %d, tid: %d] L'exit value ha come indirizzo %p btw\n", getpid(), gettid(), exit_value);
    printf("[pid: %d, tid: %d] Ho terminato l'esecuzione di func\n", getpid(), gettid());
    pthread_exit(exit_value);
}

int main() {
    pthread_t ids[NUM_OF_THREADS];

    void* exit_value;

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_create(&ids[i], NULL, &func, NULL);
    }
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        pthread_join(ids[i], &exit_value);

        if (exit_value) {
            printf("[pid: %d, tid: %d] Song o pat, l'exit value di %ld ha come indirizzo %p btw\n", getpid(), gettid(), ids[i], exit_value);
            free(exit_value);
        } else {
            printf("[pid: %d, tid: %d] Song o pat, l'exit value di %ld era NULL btw, c'è stato un errore di allcoazione\n", getpid(), gettid(), ids[i]);
        }
    }
    return 0;
}