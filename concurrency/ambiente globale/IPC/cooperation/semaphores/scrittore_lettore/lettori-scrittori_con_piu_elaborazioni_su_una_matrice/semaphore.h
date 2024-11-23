#ifndef SEMAPHORE_H
#define SEMAPHORE_H

void Wait_Sem(int semid, int numsem);
void Signal_Sem(int semid, int numsem);

#endif