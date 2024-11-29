#ifndef MATRICE_H
#define MATRICE_H

#define DIM 5

struct matrice {
    int buffer[DIM][DIM];
    int side;

    int num_scrittori;
    int num_lettori;
};

#define SYNCH 0
#define MUTEX_S 1
#define MUTEX_L 2
#define MUTEX_RISORSA 3

void inizio_scrittura(int id_sem, struct matrice* m);
void fine_scrittura(int id_sem, struct matrice* m);

void inizio_lettura(int id_sem, struct matrice* m);
void fine_lettura(int id_sem, struct matrice* m);

void scrivi(int id_sem, struct matrice* m);

void leggi_doppio(int id_sem, struct matrice* m);
void leggi_media(int id_sem, struct matrice* m);

#endif