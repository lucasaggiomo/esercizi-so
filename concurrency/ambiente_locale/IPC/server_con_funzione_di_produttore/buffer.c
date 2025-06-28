#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semaphore.h"

#include "buffer.h"

void init_buffer_arr(struct buffer_arr* b_arr, int dim, int id_sem) {
    b_arr->length = dim;     // utilizza solo i primi "length" elementi del buffer b_arr->arr (al massimo 10)
    b_arr->coda = 0;
    b_arr->testa = 0;
    b_arr->id_sem = id_sem;
}

void produci(struct buffer_arr* b_arr, struct buffer* input) {
    printf("[Server] Sto per produrre: [%d, %d]\n", input->val1, input->val2);

    Wait_Sem(b_arr->id_sem, SPAZIO_DISP);

    memcpy(&(b_arr->arr[b_arr->testa]), input, sizeof(struct buffer));

    printf("[Server] Prodotto con successo in posizione %d\n", b_arr->testa);

    b_arr->testa = (b_arr->testa + 1) % b_arr->length;

    Signal_Sem(b_arr->id_sem, MESSAGGIO_DISP);
}

void consuma(struct buffer_arr* b_arr, struct buffer* output) {
    printf("[Client] Faccio la wait sem\n");

    Wait_Sem(b_arr->id_sem, MESSAGGIO_DISP);     // Il client si sveglia qui

    printf("[Client] Consumo: coda = %d, testa = %d, length = %d\n", b_arr->coda, b_arr->testa, b_arr->length);

    memcpy(output, &b_arr->arr[b_arr->coda], sizeof(struct buffer));

    printf("[Client] Ho copiato il buffer [%d, %d] dalla posizione %d\n", output->val1, output->val2, b_arr->coda);

    b_arr->coda = (b_arr->coda + 1) % b_arr->length;
    printf("[Client] Aggiornato coda a %d\n", b_arr->coda);

    printf("[Client] Faccio la signal sem al server\n");
    Signal_Sem(b_arr->id_sem, SPAZIO_DISP);
}
