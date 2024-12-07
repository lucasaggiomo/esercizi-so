#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer_stato.h"
#include "operandi_risultati.h"

#define NUM_WORKERS 3

#define DIM_BUFFER 4

#define NUM_OPERAZIONI 6

struct param_worker {
    operandi_m* op_m;
    risultati_m* res_m;
};

void* produttore_operandi(void* arg);
void* consumatore_risultati(void* arg);
void* worker(void* arg);

int main() {
    int ret;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    operandi_m* op_m = malloc(sizeof(*op_m));
    if (!op_m) {
        perror("Errore nella malloc");
        exit(1);
    }
    init_buffer(op_m, DIM_BUFFER, sizeof(operandi_t));

    risultati_m* res_m = malloc(sizeof(*res_m));
    if (!res_m) {
        perror("Errore nella malloc");
        exit(1);
    }
    init_buffer(res_m, DIM_BUFFER, sizeof(risultato_t));

    pthread_t id_produttore_operandi;
    pthread_t id_consumatore_risultati;
    pthread_t id_workers[NUM_WORKERS];

    ret = pthread_create(&id_produttore_operandi, &attr, produttore_operandi, op_m);
    if (ret < 0) {
        perror("Errore nella pthread_create");
        exit(1);
    }

    ret = pthread_create(&id_consumatore_risultati, &attr, consumatore_risultati, res_m);
    if (ret < 0) {
        perror("Errore nella pthread_create");
        exit(1);
    }

    struct param_worker* params[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; i++) {
        struct param_worker* p = malloc(sizeof(*p));
        if (!p) {
            perror("Errore nella malloc");
            exit(1);
        }
        p->res_m = res_m;
        p->op_m = op_m;

        params[i] = p;

        ret = pthread_create(&id_workers[i], &attr, worker, p);
        if (ret < 0) {
            perror("Errore nella pthread_create");
            exit(1);
        }
    }

    pthread_join(id_produttore_operandi, NULL);
    pthread_join(id_consumatore_risultati, NULL);
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(id_workers[i], NULL);

        free(params[i]);
    }

    destroy_buffer(op_m);
    destroy_buffer(res_m);

    free(op_m);
    free(res_m);

    pthread_attr_destroy(&attr);

    return 0;
}

void* produttore_operandi(void* arg) {
    operandi_m* op_m = arg;

    srand(time(NULL) ^ pthread_self());

    for (int i = 0; i < NUM_OPERAZIONI; i++) {
        operandi_t op = {
            .operando1 = rand() % 21,
            .operando2 = rand() % 21
        };
        produci_buffer(op_m, &op);
        printf("[Produttore operandi] Ho prodotto gli operandi %d e %d\n", op.operando1, op.operando2);
    }

    pthread_exit(NULL);
}

void* consumatore_risultati(void* arg) {
    risultati_m* res_m = arg;

    for (int i = 0; i < NUM_OPERAZIONI; i++) {
        risultato_t res;
        consuma_buffer(res_m, &res);

        printf("[Consumatore risultati] Ho consumato il risultato %d\n", res);
    }

    pthread_exit(NULL);
}

void* worker(void* arg) {
    struct param_worker* p = arg;

    for (int i = 0; i < NUM_OPERAZIONI / NUM_WORKERS; i++) {
        operandi_t op;
        consuma_buffer(p->op_m, &op);
        risultato_t res = op.operando1 + op.operando2;
        printf("[Worker %ld] Ho consumato gli operandi e mi risulta %d + %d = %d\n", pthread_self() % 1000, op.operando1, op.operando2, res);

        produci_buffer(p->res_m, &res);
        printf("[Worker %ld] Ho prodotto il risultato %d\n", pthread_self() % 1000, res);
    }

    pthread_exit(NULL);
}