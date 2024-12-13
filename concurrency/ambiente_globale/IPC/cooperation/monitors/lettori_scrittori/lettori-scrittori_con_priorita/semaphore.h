#ifndef SEMAPHORE_H
#define SEMAPHORE_H

void Wait_Sem(int semid, int numsem);
void Signal_Sem(int semid, int numsem);

int Queue_Sem(int id_sem, int numsem);

#endif