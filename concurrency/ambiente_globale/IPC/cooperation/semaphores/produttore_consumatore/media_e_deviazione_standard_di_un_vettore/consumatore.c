#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "buffer_circolare.h"

int main() {
    key_t key_sem = ftok(".", 's');

    int id_sem = semget(key_sem, 2, 0);

    if (id_sem < 0) {
        perror("[Consumatore] Errore nella semget");
        exit(1);
    }

    key_t key_shm = ftok(".", 'm');

    int id_shm = shmget(key_shm, sizeof(struct BufferCircolare), 0);

    if (id_shm < 0) {
        perror("[Consumatore] Errore nella shmget");
        exit(1);
    }

    struct BufferCircolare* buf = shmat(id_shm, NULL, 0);

    if (buf == (void*)-1) {
        perror("[Consumatore] Errore nella shmat");
        exit(1);
    }

    consuma_elementi(id_sem, buf);

    sleep(1);

    consuma_elementi(id_sem, buf);

    return 0;
}