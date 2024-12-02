#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "shared.h"

int main() {
    // ottiene la coda delle richieste dai client verso i server
    key_t key_msg = ftok(".", 'q');
    int id_coda_richieste = msgget(key_msg, 0);

    if (id_coda_richieste < 0) {
        perror("Errore nell'ottenimento della coda di richieste");
        exit(1);
    }

    printf("[Server - %d] Id coda richieste ottenuto\n", getpid());

    // ottiene l'id della memoria condivisa
    key_t key_shm = ftok(".", 's');
    int id_shared_mem = shmget(key_shm, sizeof(struct shared), 0);

    if (id_shared_mem < 0) {
        perror("Errore nell'ottenimento della memoria condivisa");
        exit(1);
    }

    printf("[Server - %d] Id memoria condivisa ottenuto\n", getpid());

    // effettua l'attach della memoria condivisa
    struct shared* shm = shmat(id_shared_mem, NULL, 0);
    if (shm == (void*)-1) {
        perror("Errore nell'attach della memoria condivisa");
        exit(1);
    }

    printf("[Server - %d] Memoria condivisa attached\n", getpid());

    // nota: non è necessario effettuare il get per il mutex, in quanto l'id si trova in shm
    struct msg richiesta;

    int ret;

    for (int i = 0; i < TOTALE_RICHIESTE; i++) {
        // attende la ricezione di una richiesta
        ret = msgrcv(id_coda_richieste, &richiesta, SIZE_MSG, 0, 0);

        if (ret < 0) {
            perror("Errore nella ricezione della richiesta");
            exit(1);
        }

        printf("[Server - %d] Ho ricevuto una richiesta con num1 = %d e num2 = %d, provo ad aggiornare la memoria condivisa\n",
               getpid(), richiesta.data.num1, richiesta.data.num2);

        // aggiorna la memoria condivisa in mutua esclusione
        update_shared(shm, &richiesta.data);
    }

    return 0;
}