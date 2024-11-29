#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "buffer_circolare.h"
#include "semaphore.h"

void produci_elemento(int id_sem, struct BufferCircolare* buf) {
    Wait_Sem(id_sem, SPAZIO_DISP);

    int elemento = rand() % 10 + 1;

    printf("[Produttore] Produco %d in %d\n", elemento, buf->testa);

    buf->buffer[buf->testa] = elemento;
    buf->testa = (buf->testa + 1) % DIM;

    if (buf->testa == buf->coda) {
        Signal_Sem(id_sem, BUFFER_PIENO);
    }
}

void consuma_elementi(int id_sem, struct BufferCircolare* buf) {
    Wait_Sem(id_sem, BUFFER_PIENO);

    double media = 0.0;
    double dev_std = 0.0;

    printf("[Consumatore] Consumo tutti gli elementi\n");

    for (int i = 0; i < DIM; i++) {
        media += buf->buffer[i];
    }
    media /= DIM;
    for (int i = 0; i < DIM; i++) {
        double s = buf->buffer[i] - media;
        dev_std += s * s;
    }
    dev_std = sqrt(dev_std / (DIM - 1));

    buf->coda = buf->testa;

    printf("[Consumatore] media campionaria = %f, deviazione campionaria = %f\n", media, dev_std);

    Signal_Sem_Increment(id_sem, SPAZIO_DISP, DIM);
}