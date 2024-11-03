#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif

#define gettid() ((pid_t)syscall(SYS_gettid))

#define NUM_OF_THREADS 10
#define ARR_SIZE 10

struct buffer {
    int num1;
    int num2;
};

void* func(void* arg) {
    struct buffer* b = arg;

#ifdef DEBUG
    printf("[tid: %d] Ho iniziato l'esecuzione di func\n", gettid());
#endif

    long* dest = malloc(sizeof(*dest));
    if (!dest) {
        fprintf(stderr, "[tid: %d] Errore nell'allocazione di dest: %s\n", gettid(), strerror(errno));
        pthread_exit((void*)-1);
    }
    *dest = (long)b->num1 * b->num2;

#ifdef DEBUG
    printf("[tid: %d] Ho terminato l'esecuzione di func, con %d * %d = %ld\n", gettid(), b->num1, b->num2, *dest);
#endif

    pthread_exit(dest);
}

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf(" %d", arr[i]);
    }
    printf("\n");
}

void fill_random_array(int* dest, int size, int max) {
    if (!dest || size < 0)
        return;

    for (int i = 0; i < size; i++) {
        dest[i] = rand() % max;
    }
}
int main() {
    srand(getpid());

    struct buffer* buffers = malloc(NUM_OF_THREADS * sizeof(*buffers));
    if (!buffers) {
        perror("Errore nell'allocazione dei buffer");
        exit(EXIT_FAILURE);
    }

    int arr1[ARR_SIZE];
    int arr2[ARR_SIZE];

    fill_random_array(arr1, ARR_SIZE, 10);
    fill_random_array(arr2, ARR_SIZE, 10);

    pthread_t ids[NUM_OF_THREADS];

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        buffers[i].num1 = arr1[i];
        buffers[i].num2 = arr2[i];

        pthread_create(&ids[i], NULL, &func, buffers + i);
    }
    long sum = 0;
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        void* exit_value;
        pthread_join(ids[i], &exit_value);

        if (exit_value == (void*)-1) {
            fprintf(stderr, "[tid: %d] Song o pat, l'exit value di %ld era (void*)-1, c'è stato sicuramente un errore: %s\n", gettid(), ids[i], strerror(errno));
            sum += 0;
        } else {
            long* output = exit_value;
#ifdef DEBUG
            printf("[tid: %d] Song o pat, l'exit value %ld era %ld\n", gettid(), ids[i], *output);
#endif
            sum += *output;
            free(output);
        }
    }

#ifdef DEBUG
    printf("\n[tid %d] SONO UFFICIALMENTE TUTTI MORTI, SONO RIMASTO SOLO IO\n", gettid());
#endif
    printf("Il prodotto scalare dei seguenti array:\n");
    printf("arr1: ");
    printArray(arr1, ARR_SIZE);

    printf("arr2: ");
    printArray(arr2, ARR_SIZE);

    printf("E': %ld\n", sum);

    free(buffers);

    return 0;
}