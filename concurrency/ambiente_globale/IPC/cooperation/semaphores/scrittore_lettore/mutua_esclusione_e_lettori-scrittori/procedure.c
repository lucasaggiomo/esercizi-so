#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include "semaphore.h"

#include "header.h"

void inizio_lettura(int sem, esame_t* esame) {
    Wait_Sem(sem, MUTEX_L);

    esame->num_lettori++;
    if (esame->num_lettori == 1) {
        Wait_Sem(sem, APPELLO);
    }

    Signal_Sem(sem, MUTEX_L);
}

void fine_lettura(int sem, esame_t* esame) {
    Wait_Sem(sem, MUTEX_L);

    esame->num_lettori--;
    if (esame->num_lettori == 0) {
        Signal_Sem(sem, APPELLO);
    }

    Signal_Sem(sem, MUTEX_L);
}

void inizio_scrittura(int sem) {
    Wait_Sem(sem, APPELLO);
}

void fine_scrittura(int sem) {
    Signal_Sem(sem, APPELLO);
}

void accedi_prenotati(int sem) {
    Wait_Sem(sem, PRENOTATI);
}

void lascia_prenotati(int sem) {
    Signal_Sem(sem, PRENOTATI);
}