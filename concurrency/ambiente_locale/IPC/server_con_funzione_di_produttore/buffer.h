#ifndef BUFFER_H
#define BUFFER_H

struct buffer {
    int val1;
    int val2;
};

#define MAX_DIM 10

struct buffer_arr {
    struct buffer arr[MAX_DIM];
    int testa;
    int coda;
    int length;
    int id_sem;
};

#define SPAZIO_DISP 0
#define MESSAGGIO_DISP 1

void init_buffer_arr(struct buffer_arr* b_arr, int dim, int id_sem);

void produci(struct buffer_arr* b_arr, struct buffer* input);
void consuma(struct buffer_arr* b_arr, struct buffer* output);

#endif