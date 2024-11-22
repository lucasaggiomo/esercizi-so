#ifndef BUFFER_H
#define BUFFER_H

#define DIM 5

struct BufferCircolare {
    int buffer[DIM];
    int testa;
    int coda;
};

#define SPAZIO_DISP 0
#define BUFFER_PIENO 1

void produci_elemento(int id_sem, struct BufferCircolare* buf);
void consuma_elementi(int id_sem, struct BufferCircolare* buf);

#endif