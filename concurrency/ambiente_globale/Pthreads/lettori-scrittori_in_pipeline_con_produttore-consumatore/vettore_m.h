#ifndef VETTORE_M_H
#define VETTORE_M_H

#include "item.h"
#include <pthread.h>

enum stato {
    VUOTO,
    IN_USO,
    PIENO
};

struct vettore_m {
    struct item* vettore;
    enum stato* stato;
    int num_vuoti;
    int num_pieni;
    int dim;

    pthread_mutex_t mutex;
    pthread_cond_t cv_produttore;
    pthread_cond_t cv_consumatore;
};

void init_vettore(struct vettore_m* v, int dim);
void destroy_vettore(struct vettore_m* v);

void genera(struct vettore_m* v, struct item e);
struct item preleva(struct vettore_m* v);

#endif