#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

static int id_comandi;

struct shared {
    key_t key_client;

    pthread_mutex_t mutex;
    // NOTA: non sono necessari le variabili condition per i lettori e gli scrittori, perché in questo caso c'è un solo lettore e un solo scrittore
    // quindi è sufficiente un mutex
};

void scrivi_key(struct shared* s, key_t new_key);
key_t leggi_key(struct shared* s);

#define CODA_NON_DISPONIBILE 0
#define TERMINA_LETTORE -1

extern int gettid();

void* ricevitore(void* arg);
void* lettore(void* arg);

int main() {
    key_t key_comandi = ftok(".", 'r');
    id_comandi = Msgget(key_comandi, 0);

    // crea due thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_ricevitore;
    pthread_t id_lettore;

    struct shared* s = malloc(sizeof(*s));
    pthread_mutex_init(&s->mutex, NULL);
    s->key_client = CODA_NON_DISPONIBILE;

    Pthread_create(&id_ricevitore, &attr, ricevitore, s);
    Pthread_create(&id_lettore, &attr, lettore, s);

    Pthread_join(id_ricevitore, NULL);
    Pthread_join(id_lettore, NULL);

    pthread_mutex_destroy(&s->mutex);

    free(s);

    pthread_attr_destroy(&attr);
}

// aggiorna la chiave in mutua esclusione
void scrivi_key(struct shared* s, key_t new_key) {
    pthread_mutex_lock(&s->mutex);

    s->key_client = new_key;

    pthread_mutex_unlock(&s->mutex);
}

// legge la chiave in mutua esclusione
key_t leggi_key(struct shared* s) {
    key_t output;

    pthread_mutex_lock(&s->mutex);

    output = s->key_client;

    pthread_mutex_unlock(&s->mutex);

    return output;
}

void* ricevitore(void* arg) {
    struct shared* s = arg;

    while (1) {
        printf("[Ricevitore] Attendo un comando...\n");

        // attende un comando
        struct msg_comando com;
        Msgrcv(id_comandi, &com, SIZE_MSG_COMANDO, 0, 0);

        switch (com.type) {
            case INIZIO_LETTURA:
                printf("[Ricevitore] Ricevuto il comando INIZIO LETTURA, scrivo la chiave %d\n", com.key_coda);

                scrivi_key(s, com.key_coda);
                break;

            case FINE_LETTURA:
                printf("[Ricevitore] Ricevuto il comando FINE LETTURA, imposto CODA NON DISPONIBILE\n");

                scrivi_key(s, CODA_NON_DISPONIBILE);
                break;

            default:
                printf("[Ricevitore] Ricevuto un comando non riconosciuto, termino\n");
            case TERMINA_SESSIONE:
                printf("[Ricevitore] Ricevuto il comando TERMINA SESSIONE, scrivo la chiave TERMINA LETTORE e termino\n");

                scrivi_key(s, TERMINA_LETTORE);
                pthread_exit(NULL);
        }
    }

    pthread_exit(NULL);
}

void* lettore(void* arg) {
    struct shared* s = arg;

    while (1) {
        sleep(1);

        // legge la chiave
        key_t key_client = leggi_key(s);

        switch (key_client) {
            case CODA_NON_DISPONIBILE:
                printf("[Lettore] Letta la chiave CODA NON DISPONIBILE\n");
                break;

            case TERMINA_LETTORE:
                printf("[Lettore] Letta la chiave TERMINA LETTORE\n");
                pthread_exit(NULL);
                break;

            default:
                int id_client = Msgget(key_client, 0);
                struct msg_data msg = {
                    .type = 1,
                    .value = rand() % 11
                };
                printf("[Lettore] Letta la chiave %d, mando un messaggio con valore %d\n", key_client, msg.value);
                Msgsnd(id_client, &msg, SIZE_MSG_DATA, 0);
                break;
        }
    }
}