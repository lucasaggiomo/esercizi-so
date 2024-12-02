#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "matrice.h"
#include "semaphore.h"

void inizio_scrittura(int id_sem, struct matrice* m) {
    Wait_Sem(id_sem, MUTEX_S);

    m->num_scrittori++;
    if (m->num_scrittori == 1) {
        Wait_Sem(id_sem, SYNCH);
    }

    Signal_Sem(id_sem, MUTEX_S);
    Wait_Sem(id_sem, MUTEX_RISORSA);
}
void fine_scrittura(int id_sem, struct matrice* m) {
    Signal_Sem(id_sem, MUTEX_RISORSA);
    Wait_Sem(id_sem, MUTEX_S);

    m->num_scrittori--;
    if (m->num_scrittori == 0) {
        Signal_Sem(id_sem, SYNCH);
    }

    Signal_Sem(id_sem, MUTEX_S);
}

void inizio_lettura(int id_sem, struct matrice* m) {
    Wait_Sem(id_sem, MUTEX_L);

    m->num_lettori++;
    if (m->num_lettori == 1) {
        Wait_Sem(id_sem, SYNCH);
    }

    Signal_Sem(id_sem, MUTEX_L);
}

void fine_lettura(int id_sem, struct matrice* m) {
    Wait_Sem(id_sem, MUTEX_L);

    m->num_lettori--;
    if (m->num_lettori == 0) {
        Signal_Sem(id_sem, SYNCH);
    }

    Signal_Sem(id_sem, MUTEX_L);
}

void scrivi(int id_sem, struct matrice* m) {
    inizio_scrittura(id_sem, m);

    printf("[%d] Matrice generata:\n", getpid());
    for (int i = 0; i < m->side; i++) {
        for (int j = 0; j < m->side; j++) {
            m->buffer[i][j] = rand() % 10;
            printf("%d ", m->buffer[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    sleep(1);

    fine_scrittura(id_sem, m);
}

void leggi_doppio(int id_sem, struct matrice* m) {
    inizio_lettura(id_sem, m);

    printf("[%d] Matrice per due:\n", getpid());
    for (int i = 0; i < m->side; i++) {
        for (int j = 0; j < m->side; j++) {
            printf("%d ", m->buffer[i][j] * 2);
        }
        printf("\n");
    }
    printf("\n");

    fine_lettura(id_sem, m);
}

void leggi_media(int id_sem, struct matrice* m) {
    inizio_lettura(id_sem, m);

    int somma = 0;
    for (int i = 0; i < m->side; i++) {
        for (int j = 0; j < m->side; j++) {
            somma += m->buffer[i][j];
        }
    }
    printf("[%d] Media della matrice: %d\n", getpid(), somma);

    fine_lettura(id_sem, m);
}