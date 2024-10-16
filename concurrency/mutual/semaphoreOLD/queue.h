#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

struct queue {
    int capacity;
    int size;
    int head;
    int tail;
    int* elements;
};

void init_queue(struct queue* Q, int maxElements);

int head(const struct queue* Q);

bool enqueue(struct queue* Q, int element);

int dequeue(struct queue* Q);

void destroy_queue(struct queue* Q);

bool contains(const struct queue* Q, int element);

int get_index_of(const struct queue* Q, int element);

void print_queue(struct queue* Q, void (*print)(int));

#endif