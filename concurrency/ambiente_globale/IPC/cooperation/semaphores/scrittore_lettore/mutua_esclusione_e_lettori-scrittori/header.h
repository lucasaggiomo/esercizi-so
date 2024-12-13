#ifndef __HEADER
#define __HEADER

typedef struct tipo_esame {
    char prossimo_appello[20];
    int numero_prenotati;

    int num_lettori;
} esame_t;

#define MUTEX_L 0
#define APPELLO 1
#define PRENOTATI 2

void inizio_lettura(int sem, esame_t* esame);
void fine_lettura(int sem, esame_t* esame);
void inizio_scrittura(int sem);
void fine_scrittura(int sem);
void accedi_prenotati(int sem);
void lascia_prenotati(int sem);

#endif