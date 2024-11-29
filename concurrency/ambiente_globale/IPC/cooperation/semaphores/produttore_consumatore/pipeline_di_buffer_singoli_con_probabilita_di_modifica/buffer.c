#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "buffer.h"
#include "semaphore.h"

void produci(int id_sem, struct buffer* dest, const struct buffer* source) {
    Wait_Sem(id_sem, PRODUTTORE);

    memcpy(dest, source, sizeof(struct buffer));

    Signal_Sem(id_sem, CONSUMATORE);
}

void consuma(int id_sem, struct buffer* dest, const struct buffer* source) {
    Wait_Sem(id_sem, CONSUMATORE);

    memcpy(dest, source, sizeof(struct buffer));

    Signal_Sem(id_sem, PRODUTTORE);
}