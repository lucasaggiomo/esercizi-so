#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

#define NUM_BUFFER 2
#define NUM_SCRITTORI_PER_BUFFER 2

#define NUM_SCRITTURE 6
#define NUM_LETTURE 6

// inclusivi
#define MIN 10
#define MAX 30

void leggi_arr(int arr[], int size);
void media_arr(int arr[], int size);
void max_arr(int arr[], int size);

struct param_lettore {
    struct buffer* b_arr[NUM_BUFFER];
    void (*function)(int*, int);
};

void* scrittore(void* buffer);
void* lettore(void* arg);

int main() {
    int ret;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_scrittori[NUM_BUFFER * NUM_SCRITTORI_PER_BUFFER];
    pthread_t id_lettore_media;
    pthread_t id_lettore_max;

    // array di puntatori dei buffer
    struct buffer* b_arr[NUM_BUFFER];

    // creo i thread scrittori
    for (int i = 0; i < NUM_BUFFER; i++) {
        // alloco il buffer corrente
        b_arr[i] = malloc(sizeof(struct buffer));
        if (!b_arr[i]) {
            perror("Errore nella malloc di un buffer");
            exit(1);
        }
        init_buffer(b_arr[i]);

        for (int j = 0; j < NUM_SCRITTORI_PER_BUFFER; j++) {
            ret = pthread_create(&id_scrittori[i * NUM_SCRITTORI_PER_BUFFER + j], &attr, scrittore, b_arr[i]);
            if (ret < 0) {
                perror("Errore nella creazione del produttore");
                exit(1);
            }
        }
    }

    // alloco i parametri per i lettori
    struct param_lettore* p_media = malloc(sizeof(*p_media));
    if (!p_media) {
        perror("Errore nella malloc dei parametri del lettore della media");
        exit(1);
    }
    memcpy(p_media->b_arr, b_arr, NUM_BUFFER * sizeof(struct buffer*));
    p_media->function = media_arr;

    struct param_lettore* p_max = malloc(sizeof(*p_max));
    if (!p_max) {
        perror("Errore nella malloc dei parametri del lettore della media");
        exit(1);
    }
    memcpy(p_max->b_arr, b_arr, NUM_BUFFER * sizeof(struct buffer*));
    p_max->function = max_arr;

    // creo i thread lettori
    ret = pthread_create(&id_lettore_media, &attr, lettore, p_media);
    if (ret != 0) {
        perror("Errore nella creazione del lettore della media");
        exit(1);
    }

    ret = pthread_create(&id_lettore_max, &attr, lettore, p_max);
    if (ret != 0) {
        perror("Errore nella creazione del lettore del max");
        exit(1);
    }

    // attendo la morte
    for (int i = 0; i < NUM_BUFFER * NUM_SCRITTORI_PER_BUFFER; i++) {
        pthread_join(id_scrittori[i], NULL);
    }
    pthread_join(id_lettore_media, NULL);
    pthread_join(id_lettore_max, NULL);

    // dealloco tutto
    for (int i = 0; i < NUM_BUFFER; i++) {
        destroy_buffer(b_arr[i]);
        free(b_arr[i]);
    }
    free(p_media);
    free(p_max);

    pthread_attr_destroy(&attr);

    return 0;
}

void* scrittore(void* buffer) {
    struct buffer* b = buffer;

    srand(time(NULL) ^ pthread_self());

    for (int i = 0; i < NUM_SCRITTURE; i++) {
        int num = rand() % (MAX - MIN + 1) + MIN;
        scrivi(b, num);
        printf("[Scrittore %ld] Ho scritto %d sul buffer [%d]\n", pthread_self() % (1001 - 300) + 300, num, b->ID);
        sleep(1);
    }
}

void* lettore(void* arg) {
    struct param_lettore* p = arg;

    for (int i = 0; i < NUM_LETTURE; i++) {
        // legge tutti i buffer
        int nums[NUM_BUFFER];
        for (int j = 0; j < NUM_BUFFER; j++) {
            nums[j] = leggi(p->b_arr[j]);
        }

        // applica la funzione alle letture (ci pensa la funzione a leggere il risultato, perché altrimenti il lettore non sa cos'è)
        p->function(nums, NUM_BUFFER);

        sleep(2);
    }
}

void leggi_arr(int arr[], int size) {
    printf("[ ");
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("]\n");
}
void media_arr(int arr[], int size) {
    float media = 0.0f;
    for (int i = 0; i < size; i++) {
        media += arr[i];
    }
    media /= size;

    printf("[Lettore %ld] La media dei buffer è %.2f\t-\ti buffer erano ", pthread_self() % (1001 - 300) + 300, media);
    leggi_arr(arr, size);
}
void max_arr(int arr[], int size) {
    int max = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    printf("[Lettore %ld] Il massimo tra i buffer è %d\t-\ti buffer erano ", pthread_self() % (1001 - 300) + 300, max);
    leggi_arr(arr, size);
}