#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dvd.h"

extern pid_t gettid();

#define NUM_AFFITTUARI 4

void* affittuario(void* arg);
void* logger(void* arg);

static int logger_attivo;
pthread_mutex_t logger_mutex;

static void update_logger(int new_val) {
    pthread_mutex_lock(&logger_mutex);
    logger_attivo = new_val;
    pthread_mutex_unlock(&logger_mutex);
}
static int read_logger() {
    int val;
    pthread_mutex_lock(&logger_mutex);
    val = logger_attivo;
    pthread_mutex_unlock(&logger_mutex);
    return val;
}

int main() {
    /*
        Il programma deve istanziare 4 thread, ciascuno dei quali:
        for(int i = 1; i <= 3; i++){
            richiede una copia del film i
            attende un secondo
            la restituisce
        }
        Il programma deve inoltre istanziare un quinto thread che ogni secondo
        richiama il metodo stampa che mostra lo stato dei DVD del negozio.
    */

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct Monitor* m = malloc(sizeof(struct Monitor));
    if (!m) {
        perror("Errore nella malloc del monitor");
        exit(1);
    }
    init_monitor(m);

    int ret;

    pthread_t id_affittuari[NUM_AFFITTUARI];
    pthread_t id_logger;

    for (int i = 0; i < NUM_AFFITTUARI; i++) {
        ret = pthread_create(&id_affittuari[i], &attr, affittuario, m);
        if (ret < 0) {
            perror("Errore nella creazione di un affittuario");
            exit(1);
        }
    }

    pthread_mutex_init(&logger_mutex, NULL);
    update_logger(1);
    ret = pthread_create(&id_logger, &attr, logger, m);
    if (ret < 0) {
        perror("Errore nella creazione del logger");
        exit(1);
    }

    for (int i = 0; i < NUM_AFFITTUARI; i++) {
        ret = pthread_join(id_affittuari[i], NULL);
        if (ret < 0) {
            perror("Errore nella join di un affittuario");
            exit(1);
        }
    }

    update_logger(0);
    ret = pthread_join(id_logger, NULL);
    if (ret < 0) {
        perror("Errore nella join del logger");
        exit(1);
    }
    pthread_mutex_destroy(&logger_mutex);

    destroy_monitor(m);
    free(m);

    pthread_attr_destroy(&attr);

    return 0;
}

void* affittuario(void* arg) {
    struct Monitor* m = arg;

    for (int i = 1; i <= NUM_IDENTIFICATIVI; i++) {
        // richiede una copia del film i
        int copia = affitta(m, i);

        printf("[Affittuario %d] Ho affitato il dvd %d, copia %d\n", gettid(), i, copia);

        // attende un secondo
        sleep(1);

        // la restituisce
        restituisci(m, i, copia);

        printf("[Affittuario %d] Ho restituito il dvd %d, copia %d\n", gettid(), i, copia);
    }
}

void* logger(void* arg) {
    struct Monitor* m = arg;

    while (read_logger() == 1) {
        stampa(m);

        sleep(1);
    }

    pthread_exit(NULL);
}