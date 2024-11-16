/*
Il programma principale dovrà creare due istanze della struttura
Vettore, inizializzarle con la funzione `inizializza()` con dimensione
pari a 5, ed avviare un thread **Manager** e 4 thread **Worker**. Il
Manager dovrà utilizzare più volte la funzione `inserisci_elemento()`
per inserire in totale 20 valori interi (scelti a caso tra 0 e 5) nei
due vettori, ossia, dovrà inserire 10 valori in ciascun vettore. **Il
Manager dovrà alternare gli inserimenti nel primo e nel secondo
vettore.** Nel caso che un vettore sia pieno, il Manager deve
sospendersi fino a quando non si rende disponibile dello spazio nel
vettore.

I thread Worker eseguono in parallelo al Manager. Essi dovranno iterare
più volte, attendendo 1 secondo alla fine di ogni iterazione. Ad ogni
iterazione, un Worker **preleva una coppia di valori** usando la
funzione `preleva_elemento()` sui ciascuno dei due vettori. Nel caso che
un vettore sia vuoto, il Worker deve sospendersi fino a quando non si
renda disponibile un elemento nel vettore. Il Worker dovrà effettuare il
prodotto tra i due valori, e sommare il risultato ad una variabile
`somma` condivisa tra i thread. Inoltre, dovrà incrementare di 1 il
valore di una variabile `conteggio` condivisa tra i thread. Prima di
prelevare una coppia di valori, i thread Worker dovranno ispezionare la
variabile `conteggio`, e terminare nel caso essa sia già uguale a 10. Il
programma principale dovrà attendere la terminazione dei thread Manager
e Worker, e stampare il valore finale della somma.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vettore.h"

// funzioni wrapper pthread per gestire errori
void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg) {
    int rc = pthread_create(thread, attr, start_routine, arg);
    if (rc != 0) {
        perror("Errore nella creazione di un thread");
        exit(1);
    }
}

void Pthread_join(pthread_t thread, void** retval) {
    int rc = pthread_join(thread, retval);
    if (rc != 0) {
        perror("Errore nella join di un thread");
        exit(1);
    }
}

#define DIMENSIONE 5
#define NUM_WORKERS 4

#define NUM_INSERIMENTI_PER_VETTORE 10
#define SUP_INSERIMENTO 6

struct buffer {
    struct Vettore* primo_vettore;
    struct Vettore* secondo_vettore;
    int somma;
    int conteggio;

    pthread_mutex_t mutex_somma;
    pthread_mutex_t mutex_conteggio;
};

void init_buffer(struct buffer* b) {
    b->primo_vettore = malloc(sizeof(*b->primo_vettore));
    if (!b->primo_vettore) {
        perror("Errore nella malloc del primo vettore");
        exit(1);
    }
    inizializza(b->primo_vettore, DIMENSIONE);

    b->secondo_vettore = malloc(sizeof(*b->secondo_vettore));
    if (!b->secondo_vettore) {
        perror("Errore nella malloc del secondo vettore");
        exit(1);
    }
    inizializza(b->secondo_vettore, DIMENSIONE);

    pthread_mutex_init(&b->mutex_somma, NULL);
    b->somma = 0;

    pthread_mutex_init(&b->mutex_conteggio, NULL);
    b->conteggio = 0;
}
void destroy_buffer(struct buffer* b) {
    distruggi(b->primo_vettore);
    free(b->primo_vettore);

    distruggi(b->secondo_vettore);
    free(b->secondo_vettore);

    pthread_mutex_destroy(&b->mutex_somma);
    pthread_mutex_destroy(&b->mutex_conteggio);
}

void* manager(void* arg) {
    struct buffer* b = arg;

    for (int i = 0; i < NUM_INSERIMENTI_PER_VETTORE; i++) {
        int elemento1 = rand() % SUP_INSERIMENTO;
        int elemento2 = rand() % SUP_INSERIMENTO;

        printf("[MANAGER] Inserisco i due elementi %d e %d\n", elemento1, elemento2);
        // inserisce i due valori
        inserisci_elemento(b->primo_vettore, elemento1);
        inserisci_elemento(b->secondo_vettore, elemento2);
    }

    pthread_exit(NULL);
    return NULL;
}

void* worker(void* arg) {
    struct buffer* b = arg;

    while (1) {
        int exit = 0;
        pthread_mutex_lock(&b->mutex_conteggio);

        b->conteggio++;

        exit = (b->conteggio > NUM_INSERIMENTI_PER_VETTORE);

        pthread_mutex_unlock(&b->mutex_conteggio);

        if (exit)
            break;

        printf("[WORKER] Prelevo i due elementi\n");

        // preleva due valori
        int elemento1 = preleva_elemento(b->primo_vettore);
        int elemento2 = preleva_elemento(b->secondo_vettore);

        printf("[WORKER] Prelevati elementi %d e %d, aggiorno la somma\n", elemento1, elemento2);

        // incrementa la variabile condibisa b->somma del prodotto dei due elementi in mutua esclusione
        pthread_mutex_lock(&b->mutex_somma);

        b->somma += elemento1 * elemento2;

        pthread_mutex_unlock(&b->mutex_somma);

        sleep(1);
    }

    pthread_exit(NULL);
    return NULL;
}

int main() {
    srand(time(NULL));

    struct buffer* buff = malloc(sizeof(*buff));
    if (!buff) {
        perror("Errore nella malloc del buffer");
        exit(1);
    }
    init_buffer(buff);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t id_manager;
    pthread_t id_workers[NUM_WORKERS];

    Pthread_create(&id_manager, &attr, manager, buff);
    for (int i = 0; i < NUM_WORKERS; i++) {
        Pthread_create(&id_workers[i], &attr, worker, buff);
    }

    Pthread_join(id_manager, NULL);
    for (int i = 0; i < NUM_WORKERS; i++) {
        Pthread_join(id_workers[i], NULL);
    }

    pthread_attr_destroy(&attr);
    destroy_buffer(buff);
    free(buff);
}