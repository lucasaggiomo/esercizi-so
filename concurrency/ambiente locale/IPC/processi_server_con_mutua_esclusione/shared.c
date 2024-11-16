#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "semaphore.h"
#include "shared.h"

void update_shared(struct shared* shm, struct data* new_data) {
    Wait_Sem(shm->id_mutex, 0);

    printf("[Server - %d] Aggiorno la memoria condivisa con con num1 = %d e num2 = %d\n",
           getpid(), new_data->num1, new_data->num2);

    shm->data.num1 = new_data->num1;
    shm->data.num2 = new_data->num2;

    Signal_Sem(shm->id_mutex, 0);
}