#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <sys/types.h>

void send_sincrona(int id_coda, int id_rts, int id_ots, void* msg, size_t size_msg);
void receive_sincrona(int id_coda, int id_rts, int id_ots, void* msg, size_t size_msg);

#endif