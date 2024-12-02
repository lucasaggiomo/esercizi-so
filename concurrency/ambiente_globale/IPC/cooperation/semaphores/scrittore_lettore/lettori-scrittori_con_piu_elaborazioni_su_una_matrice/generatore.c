#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "matrice.h"

int main() {
    key_t key_sem = ftok(".", 's');
    int id_sem = semget(key_sem, 4, 0);
    if (id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }

    key_t key_shm = ftok(".", 'm');
    int id_shm = shmget(key_shm, sizeof(struct matrice), 0);
    if (id_shm < 0) {
        perror("Errore nella shmget della matrice");
        exit(1);
    }

    struct matrice* m = shmat(id_shm, NULL, 0);
    if (m == (void*)-1) {
        perror("Errore nella shmat della matrice");
        exit(1);
    }

    key_t key_stop = ftok(".", 'S');
    int id_stop = shmget(key_stop, sizeof(int), IPC_CREAT | 0644);
    if (id_stop < 0) {
        perror("Errore nella shmget dello stop");
        exit(1);
    }

    int* stop = shmat(id_stop, NULL, 0);
    if (stop == (void*)-1) {
        perror("Errore nella shmat dello stop");
        exit(1);
    }

    srand(getpid());

    while (*stop == 0) {
        /* Un processo **generatore**, che periodicamente (ogni 3 secondi)
        aggiorna il contenuto della matrice con valori casuali (scelti tra 0
        e 9). L'operazione di aggiornamento dovrà durare 1 secondo, da
        simulare mediante la primitiva `sleep()`. */

        // aggiornamento
        scrivi(id_sem, m);

        sleep(3);
    }

    return 0;
}