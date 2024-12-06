#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "header.h"

int main(int argc, char* argv[]) {
    key_t key_sem = ftok(".", 's');
    int id_sem = semget(key_sem, 3, 0);
    if (id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }
    key_t key_shm = ftok(".", 'e');
    int id_shm = shmget(key_shm, sizeof(esame_t), 0);
    if (id_shm < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    esame_t* e = shmat(id_shm, NULL, 0);
    if (e == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    if (argc <= 1) {
        fprintf(stderr, "[Docente] Argomento numero appelli non trovato\n");
    }
    int num_appelli = atoi(argv[1]);

    /*
        Per 3 volte, il processo docente dovrà
        aggiornare la data di esame (si usino delle stringhe a piacere),
        attendere 3 secondi, stampare ed azzerare il numero di prenotati (la
        stampa e l'azzeramento avvengano nella stessa sezione critica).
    */
    srand(getpid());

    for (int i = 0; i < num_appelli; i++) {
        inizio_scrittura(id_sem);
        char random_date[11];
        // per semplicità non considero la validità della data, ovvero il giorno casuale da 0 a 30 a prescindere dal mese
        snprintf(random_date, sizeof(random_date), "%d/%d/%d", rand() % 30, rand() % 12, (rand() % (2025 - 2021) + 2021));

        printf("[Docente %d] Studenti coglioni, il prossimo appello è alla data seguente: %s\n", getpid(), random_date);

        strncpy(e->prossimo_appello, random_date, sizeof(e->prossimo_appello));

        fine_scrittura(id_sem);

        sleep(3);

        accedi_prenotati(id_sem);
        inizio_lettura(id_sem, e);

        printf("[Docente %d] Ho constatato che i prenotati sono %d... E IO SPOSTO L'APPELLO AHAHAHAH\n", getpid(), e->numero_prenotati);
        e->numero_prenotati = 0;

        fine_lettura(id_sem, e);
        lascia_prenotati(id_sem);
    }

    return 0;
}