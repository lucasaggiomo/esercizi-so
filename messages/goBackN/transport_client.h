#ifndef TRANSPORT_CLIENT_H
#define TRANSPORT_CLIENT_H

#include <pthread.h>

#include "transport.h"

extern int gettid();

void transport_client(int id_rts_ots, int id_app_dati);

void get_queues();

void* receiver_and_producer(void* arg);
void* sender(void* arg);
void* receiver_ack(void* arg);

#endif