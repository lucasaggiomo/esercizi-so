#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#define NUM_RICHIESTE 5

int main() {
    key_t key_richeste = ftok(".", 'q');
    int id_richieste = Msgget(key_richeste, 0);

    key_t key_risposte = ftok(".", 'a');
    int id_risposte = Msgget(key_risposte, 0);

    pid_t pid = getpid();

    srand(pid);

    for (int i = 0; i < NUM_RICHIESTE; i++) {
        struct msg_richiesta richiesta = {
            .type = 1,
            .pid = pid,
            .values = { rand() % 11, rand() % 11 }
        };

        printf("[Client %d] Invio una richiesta [%d, %d]\n", pid, richiesta.values[0], richiesta.values[1]);

        Msgsnd(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0);

        struct msg_risposta risposta;
        Msgrcv(id_risposte, &risposta, SIZE_MSG_RISPOSTA, pid, 0);

        printf("[Client %d] Ho ricevuto la risposta: %d * %d = %d\n", pid, richiesta.values[0], richiesta.values[1], risposta.result);
    }

    return 0;
}