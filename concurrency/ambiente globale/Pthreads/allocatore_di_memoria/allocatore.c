#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "allocatore.h"

#define _GNU_SOURCE

#include "allocatore.h"

void AllocInit(struct Allocatore* a) {
    pthread_mutex_init(&a->mutex, NULL);
    pthread_cond_init(&a->cv_richiesta, NULL);

    for (int i = 0; i < SIZE_MEMORY; i++) {
        a->stato[i] = LIBERO;
    }
}

void AllocDestroy(struct Allocatore* a) {
    pthread_mutex_destroy(&a->mutex);
    pthread_cond_destroy(&a->cv_richiesta);
}

// restituisce l'offset nella memoria del primo blocco libero di almeno n byte, -1 se non presente
int first_fit(struct Allocatore* a, int n) {
    // verifica se c'è spazio per l'allocazione
    int curr_spazio_contiguo = 0;
    int indice_current_blocco = 0;

    for (int i = 0; i < SIZE_MEMORY; i++) {
        switch (a->stato[i]) {
            case LIBERO:
                curr_spazio_contiguo++;
                if (curr_spazio_contiguo == n)
                    return indice_current_blocco;
                break;
            case OCCUPATO:
                curr_spazio_contiguo = 0;
                indice_current_blocco = i + 1;
                break;
            default:
                perror("Stato della memoria non valido");
                exit(1);
        }
    }
    return -1;
}

// alloca n caratteri (first-fit):
// alloca il primo blocco di caratteri sufficientemente grande
// (marcando tali caratteri come OCCUPATO)
// e restituisce un puntatore al primo carattere del blocco
char* getMemoria(struct Allocatore* a, int n) {
    int offset;

    pthread_mutex_lock(&a->mutex);

    while ((offset = first_fit(a, n)) == -1) {
        printf("[%d] Spazio non disponibile per %d byte, attendo che si liberi memoria...\n", gettid(), n);
        pthread_cond_wait(&a->cv_richiesta, &a->mutex);
    }

    printf("[%d] Alloco %d byte in (", gettid(), n);

    // imposto n byte a partire dal blocco come occupati
    for (int i = 0; i < n; i++) {
        a->stato[offset + i] = OCCUPATO;

        printf("%d", offset + i);
        if (i != n - 1)
            printf("-");
    }

    printf(")\n");

    printMemoryState(a);

    pthread_mutex_unlock(&a->mutex);

    return &a->memoria[offset];
}

// dealloca n caratteri
void releaseMemoria(struct Allocatore* a, char* blocco, int n) {
    if (n < 0) {
        perror("Errore: la dimensione del blocco era negativa");
        return;
    }

    pthread_mutex_lock(&a->mutex);

    // cerco l'offset del blocco all'interno della memoria
    int offset = 0;
    while (&a->memoria[offset] != blocco && offset < SIZE_MEMORY) {
        offset++;
    }

    if (offset == SIZE_MEMORY) {
        perror("Errore: blocco non trovato nella memoria");
        return;
    }

    if (offset + n > SIZE_MEMORY) {
        perror("Errore: il blocco esce al di fuori della memoria");
        return;
    }

    printf("[%d] Dealloco il blocco di memoria (", gettid());

    // imposto la memoria del blocco come libera
    for (int j = 0; j < n; j++) {
        a->stato[offset + j] = LIBERO;

        printf("%d", offset + j);
        if (j != n - 1)
            printf("-");
    }
    printf(")\n");

    printMemoryState(a);

    pthread_cond_broadcast(&a->cv_richiesta);

    pthread_mutex_unlock(&a->mutex);
}

void printMemoryState(struct Allocatore* a) {
    printf("\nMemoria: [");
    for (int i = 0; i < SIZE_MEMORY; i++) {
        printf(" ");
        switch (a->stato[i]) {
            case LIBERO:
                printf("-");
                break;
            case OCCUPATO:
                printf("#");
                break;
            default:
                printf("?");
                break;
        }
    }
    printf(" ]\n\n");
}