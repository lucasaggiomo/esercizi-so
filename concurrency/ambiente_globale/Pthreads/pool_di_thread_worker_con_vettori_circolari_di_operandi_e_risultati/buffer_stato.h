#ifndef BUFFER_STATO_H
#define BUFFER_STATO_H

#include <pthread.h>

typedef unsigned char byte;

enum stato {
    LIBERO,
    OCCUPATO,
    IN_USO
};

struct buffer_stato_m {
    void* vettore_buffer;
    enum stato* vettore_stato;
    int num_liberi;
    int num_occupati;

    int dim;

    size_t element_size;

    pthread_mutex_t mutex;
    pthread_cond_t cv_produttore;
    pthread_cond_t cv_consumatore;
};

void init_buffer(struct buffer_stato_m* m, int dim, size_t element_size);
void destroy_buffer(struct buffer_stato_m* m);

void* item_at(struct buffer_stato_m* m, int index);
int first_index_of(struct buffer_stato_m* m, enum stato stato);

void produci_buffer(struct buffer_stato_m* m, void* item);
void consuma_buffer(struct buffer_stato_m* m, void* output);

#endif