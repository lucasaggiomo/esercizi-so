#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "buffer.h"

#include "messaggi.h"

#define NUM_FIGLI 3

void figlio(struct buffer* b, int id_richieste, int num_richieste);

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Errore! Riesegui il programma inserendo il numero di richieste da dover attendere");
        exit(1);
    }
    int totale_richieste = atoi(argv[1]);

    int id_buffer = shmget(IPC_PRIVATE, sizeof(struct buffer), IPC_CREAT | 0644);
    if (id_buffer < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    struct buffer* b = shmat(id_buffer, NULL, 0);
    if (b == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    key_t key_richieste = ftok(".", 'r');
    int id_richieste = msgget(key_richieste, IPC_CREAT | 0644);
    if (id_richieste < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    init_buffer(b);

    pid_t pid;

    int richieste_finora = 0;
    int num_richieste_per_figlio = totale_richieste / NUM_FIGLI;
    for (int i = 0; i < NUM_FIGLI; i++) {
        // all'ultimo figlio dà tutte le richieste rimanenti (per gestire il caso in cui totale_richieste non è divisibile per NUM_FIGLI)
        int richieste = (i == NUM_FIGLI - 1) ? totale_richieste - richieste_finora : num_richieste_per_figlio;
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            figlio(b, id_richieste, richieste);

            printf("[Figlio %d] Terminazione\n", getpid());
            exit(0);
        }
        richieste_finora += num_richieste_per_figlio;
    }

    for (int i = 0; i < NUM_FIGLI; i++) {
        wait(NULL);
    }

    // legge non in mutua esclusione: è certo che non ci sono scrittori
    printf("[Padre] Numero messaggi: %d, Totale accumulato: %d\n", b->numero_messaggi, b->totale);

    destroy_buffer(b);

    msgctl(id_richieste, IPC_RMID, NULL);
    shmctl(id_buffer, IPC_RMID, NULL);

    printf("[Padre] Terminazione\n");

    return 0;
}

void figlio(struct buffer* b, int id_richieste, int num_richieste) {
    for (int i = 0; i < num_richieste; i++) {
        // attende una richiesta dal client
        struct msg_richiesta richiesta;
        printf("[Figlio %d] Attesa richiesta...\n", getpid());
        int ret = msgrcv(id_richieste, &richiesta, SIZEOF_MSG(struct msg_richiesta), 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        printf("[Figlio %d] Ho ricevuto una richiesta %d, aggiorno il buffer\n", getpid(), richiesta.quantita);
        scrivi(b, richiesta.quantita);
    }
}
