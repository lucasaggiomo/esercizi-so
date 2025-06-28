#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "aeroporto.h"

#define PRINTF_SEM 0

void set_id_printf_sem(int value);
int printf_sem(const char* format, ...);

void gate(int id_avvisi, int num_gate);
void aggiornatore(int id_avvisi, struct aeroporto* a);
void display(struct aeroporto* a);

#endif