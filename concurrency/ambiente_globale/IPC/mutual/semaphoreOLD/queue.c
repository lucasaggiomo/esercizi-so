#include "queue.h"
#include <stdbool.h>
#include <stdlib.h>

// crea una Queue
void init_queue(struct queue* Q, int maxElements) {
    // struct queue* Q = malloc(sizeof(struct queue));  // responsabilità del chiamante
    if (!Q)
        return;

    Q->elements = malloc(sizeof(int) * maxElements);
    if (!Q->elements)
        return;

    Q->capacity = maxElements;
    Q->size = 0;
    Q->head = 0;
    Q->tail = -1;
}

int head(const struct queue* Q) {
    if (Q->size == 0)
        return -1;

    return Q->elements[Q->head];
}

// aggiunge un elemento alla coda
bool enqueue(struct queue* Q, int element) {
    if (Q->size == Q->capacity)
        return false;

    // printf("Added element %d\n", element);
    Q->tail = (Q->tail + 1) % Q->capacity;

    Q->elements[Q->tail] = element;
    Q->size++;
    return true;
}

// rimuove un elemento dalla coda (quello in testa)
int dequeue(struct queue* Q) {
    if (Q->size == 0)
        return -1;

    int element = Q->elements[Q->head];

    Q->head = (Q->head + 1) % Q->capacity;
    Q->size--;

    return element;
}

void destroy_queue(struct queue* Q) {
    if (!Q)
        return;

    free(Q->elements);
    // free(Q);  responsabilità del chiamante
}

bool contains(const struct queue* Q, int element) {
    if (!Q)
        return false;

    int head = Q->head;
    for (int i = 0; i < Q->size; i++) {
        if (Q->elements[head] == element)
            return true;

        head = (head + 1) % Q->capacity;
    }
    return false;
}

int get_index_of(const struct queue* Q, int element) {
    if (!Q)
        return -1;

    int head = Q->head;
    for (int i = 0; i < Q->size; i++) {
        if (Q->elements[head] == element)
            return i;

        head = (head + 1) % Q->capacity;
    }
    return -1;
}

void print_queue(struct queue* Q, void (*print)(int)) {
    if (!Q)
        return;

    int head = Q->head;
    for (int i = 0; i < Q->size; i++) {
        print(Q->elements[head]);
        head = (head + 1) % Q->capacity;
    }
}