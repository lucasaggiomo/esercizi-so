#ifndef SEMAPHORE_H
#define SEMAPHORE_H

void Wait_Sem(int id_sem, int numsem);
void Signal_Sem(int id_sem, int numsem);
void Signal_Sem_Increment(int id_sem, int numsem, int increment);

#endif