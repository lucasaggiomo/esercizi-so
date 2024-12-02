#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "monitorLS.h"

/*
    Il programma dovrà creare 3 processi lettori, 2 processi scrittori
    lenti, e 2 processi scrittori veloci. Ciascuno di essi dovrà chiamare la
    corrispondente funzione del monitor per 3 volte, attendendo 3 secondi
    tra le chiamate. Gli scrittori dovranno generare un valore casuale
    intero tra 0 e 10 per la scrittura. I processi dovranno stampare a video
    i valori letti e scritti. Il processo padre dovrà attendere la
    terminazione dei figli e terminare a sua volta. È richiesto che il
    codice sia contenuto all'interno di un unico eseguibile.
*/

#define TOTALE_LETTORI 3
#define TOTALE_SCRITTORI_VELOCI 2
#define TOTALE_SCRITTORI_LENTI 2

#define TOTALE_ITERAZIONI 3

void lettore(struct MonitorLS* m) {
    for (int i = 0; i < TOTALE_ITERAZIONI; i++) {
        int value = leggi(m);

        printf("[Lettore %d] Ho letto %d\n", getpid(), value);

        sleep(3);
    }
}

void scrittore_lento(struct MonitorLS* m) {
    for (int i = 0; i < TOTALE_ITERAZIONI; i++) {
        int value = rand() % 11;

        scrivi_lento(m, value);

        printf("[Scrittore lento %d] Ho scritto %d\n", getpid(), value);

        sleep(3);
    }
}

void scrittore_veloce(struct MonitorLS* m) {
    for (int i = 0; i < TOTALE_ITERAZIONI; i++) {
        int value = rand() % 11;

        scrivi_veloce(m, value);

        printf("[Scrittore veloce %d] Ho scritto %d\n", getpid(), value);

        sleep(3);
    }
}

int main() {
    int id_shm = shmget(IPC_PRIVATE, sizeof(struct MonitorLS), IPC_CREAT | 0644);
    if (id_shm < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    struct MonitorLS* m = shmat(id_shm, NULL, 0);
    if (m == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    inizializza(m);

    pid_t pid;

    for (int i = 0; i < TOTALE_LETTORI; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            lettore(m);
            exit(0);
        }
    }

    for (int i = 0; i < TOTALE_SCRITTORI_LENTI; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            scrittore_lento(m);
            exit(0);
        }
    }

    for (int i = 0; i < TOTALE_SCRITTORI_VELOCI; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            scrittore_veloce(m);
            exit(0);
        }
    }

    for (int i = 0; i < TOTALE_LETTORI + TOTALE_SCRITTORI_LENTI + TOTALE_SCRITTORI_VELOCI; i++) {
        wait(NULL);
    }

    shmctl(id_shm, IPC_RMID, NULL);

    distruggi(m);
    
    return 0;
}