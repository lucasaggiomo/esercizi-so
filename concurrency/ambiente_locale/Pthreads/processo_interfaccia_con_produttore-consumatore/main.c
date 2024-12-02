#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "procedure.h"
#include "wrapper.h"

#define NUM_CLIENT 3
#define NUM_RICHIESTE 3

#define DIMENSIONE_BUFFER 5

int main() {
    printf("[Padre] Alloco le code\n");

    int id_richieste_client = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    int id_risposte = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    int id_rts_ots = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    int id_richieste_server = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);

    printf("[Padre] Creo i processi\n");

    pid_t pid;

    for (int i = 0; i < NUM_CLIENT; i++) {
        pid = Fork();
        if (pid == 0) {
            client(id_richieste_client, id_risposte, NUM_RICHIESTE);
            exit(0);
        }
    }

    pid = Fork();
    if (pid == 0) {
        interfaccia(id_richieste_client, id_risposte, id_rts_ots, id_richieste_server, DIMENSIONE_BUFFER);
        exit(0);
    }

    pid = Fork();
    if (pid == 0) {
        server(id_rts_ots, id_richieste_server);
        exit(0);
    }

    printf("[Padre] Attendo terminazione dei client...\n");

    for (int i = 0; i < NUM_CLIENT; i++) {
        wait(NULL);
    }

    int time_to_wait = 2 * DIMENSIONE_BUFFER;

    printf("[Padre] I client hanno terminato, mando un messaggio di terminazione e poi attendo %d secondi...\n", time_to_wait);

    // mando un messaggio di terminazione all'interfaccia
    struct msg_richiesta terminazione = {
        .type = 1,
        .pid = PID_TERMINAZIONE
    };
    Msgsnd(id_richieste_client, &terminazione, SIZE_MSG_RICHIESTA, 0);

    // attendo che il messaggio di terminazione si propaghi al server
    sleep(time_to_wait);

    printf("[Padre] Dealloco le code\n");

    Msgctl(id_richieste_client, IPC_RMID, NULL);
    Msgctl(id_risposte, IPC_RMID, NULL);
    Msgctl(id_rts_ots, IPC_RMID, NULL);
    Msgctl(id_richieste_server, IPC_RMID, NULL);

    return 0;
}