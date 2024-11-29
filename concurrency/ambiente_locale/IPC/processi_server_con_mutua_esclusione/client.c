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

int main() {
    // ottiene la coda delle richieste dai client verso i server
    key_t key_msg = ftok(".", 'q');
    int id_coda_richieste = msgget(key_msg, 0);

    if (id_coda_richieste < 0) {
        perror("Errore nell'ottenimento della coda di richieste");
        exit(1);
    }

    printf("[Client - %d] Id coda richieste ottenuto\n", getpid());

    struct msg richiesta = {
        .type = 1
    };
    int ret;

    srand(getpid());

    for (int i = 0; i < TOTALE_RICHIESTE; i++) {
        // crea il dato
        richiesta.data.num1 = rand() % 10;
        richiesta.data.num2 = rand() % 10;

        // invia una richiesta sulla coda delle richieste
        ret = msgsnd(id_coda_richieste, &richiesta, SIZE_MSG, 0);

        if (ret < 0) {
            perror("Errore nell'invio della richiesta");
        }

        printf("[Client - %d] Inviata una richiesta con num1 = %d e num2 = %d\n", getpid(), richiesta.data.num1, richiesta.data.num2);

        sleep(rand() % 2 + 1);
    }

    return 0;
}