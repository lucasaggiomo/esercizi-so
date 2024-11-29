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

    while (*stop == 0) {
        /*Due processi **analizzatore**, che periodicamente (ogni 2 secondi)
        leggono il contenuto della matrice, calcolano il valore medio degli
        elementi, e stampano a video il risultato.*/
        leggi_media(id_sem, m);
        sleep(2);
    }
    return 0;
}