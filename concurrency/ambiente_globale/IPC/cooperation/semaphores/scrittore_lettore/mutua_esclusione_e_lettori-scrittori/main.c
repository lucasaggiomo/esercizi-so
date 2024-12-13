#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#include "header.h"

#define NUM_DOCENTI 1
#define NUM_STUDENTI 10

#define NUM_APPELLI 3

int main() {
    key_t key_sem = ftok(".", 's');
    int id_sem = semget(key_sem, 3, IPC_CREAT | 0644);
    if (id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }

    // inizializzazione semafori
    semctl(id_sem, MUTEX_L, SETVAL, 1);
    semctl(id_sem, APPELLO, SETVAL, 1);
    semctl(id_sem, PRENOTATI, SETVAL, 1);

    key_t key_shm = ftok(".", 'e');
    int id_shm = shmget(key_shm, sizeof(esame_t), IPC_CREAT | 0644);
    if (id_shm < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    esame_t* e = shmat(id_shm, NULL, 0);
    if (e == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    // inizializzazione shm
    e->numero_prenotati = 0;
    e->prossimo_appello[0] = '\0';
    e->num_lettori = 0;

    for (int i = 0; i < NUM_DOCENTI; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            char buff[20];
            snprintf(buff, sizeof(buff), "%d", NUM_APPELLI);
            execl("./docente", "docente", buff, NULL);
            perror("Errore nella execl docente");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_STUDENTI; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            execl("./studente", "studente", NULL);
            perror("Errore nella execl studente");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_DOCENTI + NUM_STUDENTI; i++) {
        int status;
        wait(&status);
        if (status != 0) {
            fprintf(stderr, "Attenzione! Un figlio ha terminato con stato %d\n", status);
        }
    }

    shmctl(id_shm, IPC_RMID, NULL);
    semctl(id_sem, 0, IPC_RMID, NULL);

    return 0;
}