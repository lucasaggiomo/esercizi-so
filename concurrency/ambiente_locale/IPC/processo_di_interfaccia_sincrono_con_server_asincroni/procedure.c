#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"

#include "procedure.h"

void client(int id_rts, int id_ots, int id_richieste, int num_richieste) {
    int ret;

    srand(getpid());

    for (int i = 0; i < num_richieste; i++) {
        struct msg_richiesta richiesta = {
            .type = RICHIESTA_CLIENT,
            .pid_client = getpid(),
            .value = rand() % 10
        };

        // la prima richiesta è sincrona
        if (i == 0) {
            send_sincrona(id_rts, id_ots, id_richieste, &richiesta);
        } else {
            printf("[Client %d] Invio la richiesta [%d, %d]\n", getpid(), richiesta.pid_client, richiesta.value);
            ret = msgsnd(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0);
            if (ret < 0) {
                perror("Errore nella msgsnd");
                exit(1);
            }
        }
    }

    // invia un messaggio di terminazione della comunicazione all'interfaccia
    struct msg_richiesta richiesta = {
        .type = TERMINA_COMUNICAZIONE,
        .pid_client = getpid(),
        .value = -1
    };

    printf("[Client %d] Invio la richiesta di TERMINA COMUNICAZIONE\n", getpid());
    ret = msgsnd(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }

    printf("[Client %d] Terminazione\n", getpid());
}

void interfaccia(int id_rts, int id_ots, int id_richieste, int id_servers[], int num_servers) {
    int index_server = 0;

    while (1) {
        int ret;

        printf("[Interfaccia %d] Attendo una REQUEST TO SEND o un messaggio di TERMINA PROCESSO\n", getpid());
        struct msg_rts rts;
        ret = msgrcv(id_rts, &rts, SIZE_MSG_RTS, 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        if (rts.type == TERMINA_PROCESSO) {
            printf("[Interfaccia %d] Ho ricevuto una richiesta di TERMINA PROCESSO [%d]\n", getpid(), rts.pid);
            break;
        }

        printf("[Interfaccia %d] Ho ricevuto una REQUEST TO SEND dal client [%d], invio una OK TO SEND\n", getpid(), rts.pid);
        struct msg_ots ots = {
            .type = rts.pid
        };
        ret = msgsnd(id_ots, &ots, SIZE_MSG_OTS, 0);
        if (ret < 0) {
            perror("Errore nella msgsnd");
            exit(1);
        }

        while (1) {
            struct msg_richiesta richiesta;
            int ret = msgrcv(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0, 0);
            if (ret < 0) {
                perror("Errore nella msgrcv");
                exit(1);
            }

            if (richiesta.type == TERMINA_COMUNICAZIONE) {
                printf("[Interfaccia %d] Ho ricevuto una richiesta di TERMINA COMUNICAZIONE da parte del client %d\n", getpid(), richiesta.pid_client);
                break;
            }

            printf("[Interfaccia %d] Ho ricevuto la richiesta [%d, %d], la invio al server %d\n", getpid(), richiesta.pid_client, richiesta.value, index_server);

            ret = msgsnd(id_servers[index_server], &richiesta, SIZE_MSG_RICHIESTA, 0);
            if (ret < 0) {
                perror("Errore nella msgsnd");
                exit(1);
            }

            index_server = (index_server + 1) % num_servers;
        }

        printf("[Interfaccia %d] Cambio client (mi rimetto in ascolto per una REQUEST TO SEND oppure un messaggio di TERMINA PROCESSO)\n", getpid());
    }

    printf("[Interfaccia %d] Terminazione\n", getpid());
}

void server(int id_richieste) {
    while (1) {
        struct msg_richiesta richiesta;
        int ret = msgrcv(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        if (richiesta.type == TERMINA_PROCESSO) {
            printf("[Server %d] Ho ricevuto una richiesta di TERMINA PROCESSO da [%d]\n", getpid(), richiesta.pid_client);
            break;
        }

        printf("[Server %d] Ho ricevuto la richiesta [%d, %d]\n", getpid(), richiesta.pid_client, richiesta.value);
    }

    printf("[Server %d] Terminazione\n", getpid());
}