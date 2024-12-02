#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define _GNU_SOURCE

#include "messaggi.h"
#include "shared.h"
#include "wrapper.h"

#include "procedure.h"

extern pid_t gettid(void);

void client(int id_richieste, int id_risposte, int num_richieste) {
    printf("[Client %d] In esecuzione\n", getpid());

    pid_t pid = getpid();

    srand(pid);

    for (int i = 0; i < num_richieste; i++) {
        struct msg_richiesta richiesta = {
            .type = 1,
            .pid = pid,
            .data = rand() % 20
        };

        printf("[Client %d] Invio una richiesta all'interfaccia, con valore %d\n", pid, richiesta.data);

        Msgsnd(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0);

        struct msg_risposta risposta;
        Msgrcv(id_risposte, &risposta, SIZE_MSG_RISPOSTA, pid, 0);

        switch (risposta.response) {
            case CONFERMA_TRASMISSIONE:
                printf("[Client %d] Ho ricevuto una risposta di CONFERMA riguardo la richiesta %d\n", pid, richiesta.data);
                break;
            case RIFIUTO_TRASMISSIONE:
                printf("[Client %d] Ho ricevuto una risposta di RIFIUTO riguardo la richiesta %d\n", pid, richiesta.data);
                break;
            default:
                perror("[Client] Risposta con response non riconosciuto");
                exit(1);
        }
    }

    printf("[Client %d] Terminazione\n", getpid());
}

struct param_ricevitore {
    int id_richieste_client;
    int id_risposte;
    struct shared* sh;
};

struct param_mittente {
    int id_rts_ots;
    int id_richieste_server;
    struct shared* sh;
};

void interfaccia(int id_richieste_client, int id_risposte, int id_rts_ots, int id_richieste_server, int dim_buffer) {
    printf("[Interfaccia %d] In esecuzione\n", getpid());

    pthread_t id_ricevitore;
    pthread_t id_mittente;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct shared* sh = Malloc(sizeof(*sh));
    init_shared(sh, dim_buffer);

    struct param_ricevitore* p_ricevitore = Malloc(sizeof(*p_ricevitore));
    p_ricevitore->id_richieste_client = id_richieste_client;
    p_ricevitore->id_risposte = id_risposte;
    p_ricevitore->sh = sh;

    struct param_mittente* p_mittente = Malloc(sizeof(*p_mittente));
    p_mittente->id_rts_ots = id_rts_ots;
    p_mittente->id_richieste_server = id_richieste_server;
    p_mittente->sh = sh;

    Pthread_create(&id_ricevitore, &attr, ricevitore, p_ricevitore);
    Pthread_create(&id_mittente, &attr, mittente, p_mittente);

    Pthread_join(id_ricevitore, NULL);
    free(p_ricevitore);

    Pthread_join(id_mittente, NULL);
    free(p_mittente);

    destroy_shared(sh);
    free(sh);

    pthread_attr_destroy(&attr);

    printf("[Interfaccia %d] Terminazione\n", getpid());
}

void* ricevitore(void* arg) {
    printf("[Ricevitore %d] In esecuzione\n", gettid());

    struct param_ricevitore* p = arg;

    struct msg_richiesta richiesta;
    while (1) {
        Msgrcv(p->id_richieste_client, &richiesta, SIZE_MSG_RICHIESTA, 0, 0);

        // controlla se è un messaggio di terminazione
        if (richiesta.pid == PID_TERMINAZIONE) {
            printf("[Ricevitore %d] Ho ricevuto un messaggio di TERMINAZIONE\n", gettid());
            break;
        }

        printf("[Ricevitore %d] Ho ricevuto una richiesta dal client %d, con valore %d, provo a produrre\n", gettid(), richiesta.pid, richiesta.data);

        struct msg_risposta risposta = {
            .type = richiesta.pid
        };
        if (produci_se_puoi(p->sh, &richiesta)) {
            printf("[Ricevitore %d] Produzione effettuata con successo, invio una risposta di CONFERMA al client\n", gettid());
            risposta.response = CONFERMA_TRASMISSIONE;
        } else {
            printf("[Ricevitore %d] Produzione fallita, invio una risposta di RIFIUTO al client\n", gettid());
            risposta.response = RIFIUTO_TRASMISSIONE;
        }
        Msgsnd(p->id_risposte, &risposta, SIZE_MSG_RISPOSTA, 0);
    }

    // se è qui ha ricevuto un messaggio di terminazione
    // produce il messaggio di terminazione sul buffer (attendendo che gli altri messaggi vengano trasmessi al server
    produci(p->sh, &richiesta);

    printf("[Ricevitore %d] Terminazione\n", gettid());

    pthread_exit(NULL);
}

void* mittente(void* arg) {
    printf("[Mittente %d] In esecuzione\n", gettid());

    struct param_mittente* p = arg;

    while (1) {
        struct msg_richiesta richiesta;
        consuma(p->sh, &richiesta);

        // controlla se è un messaggio di terminazione
        if (richiesta.pid == PID_TERMINAZIONE) {
            printf("[Mittente %d] Ho ricevuto un messaggio di TERMINAZIONE\n", gettid());
        }

        printf("[Mittente %d] Ho consumato la richiesta del client %d con valore %d, mando REQUEST TO SEND al server\n", gettid(), richiesta.pid, richiesta.data);

        struct msg_rts_ots buff = {
            .type = REQUEST_TO_SEND
        };
        Msgsnd(p->id_rts_ots, &buff, SIZE_MSG_RTS_OTS, 0);

        Msgrcv(p->id_rts_ots, &buff, SIZE_MSG_RTS_OTS, OK_TO_SEND, 0);

        printf("[Mittente %d] Ho ricevuto l'OK TO SEND dal server, invio la richiesta del client %d con valore %d\n", gettid(), richiesta.pid, richiesta.data);

        Msgsnd(p->id_richieste_server, &richiesta, SIZE_MSG_RICHIESTA, 0);

        if (richiesta.pid == PID_TERMINAZIONE)
            break;
    }

    printf("[Mittente %d] Terminazione\n", gettid());

    // se è qui ha ricevuto un messaggio di terminazione e lo ha inoltrato al server
    pthread_exit(NULL);
}

void server(int id_rts_ots, int id_richieste) {
    printf("[Server %d] In esecuzione\n", getpid());

    while (1) {
        struct msg_rts_ots buff;
        Msgrcv(id_rts_ots, &buff, SIZE_MSG_RTS_OTS, 0, 0);

        printf("[Server %d] Ho ricevuto una REQUEST TO SEND, invio una OK TO SEND\n", getpid());

        buff.type = OK_TO_SEND;
        Msgsnd(id_rts_ots, &buff, SIZE_MSG_RTS_OTS, 0);

        struct msg_richiesta richiesta;
        Msgrcv(id_richieste, &richiesta, SIZE_MSG_RICHIESTA, 0, 0);

        // controlla se è un messaggio di terminazione
        if (richiesta.pid == PID_TERMINAZIONE) {
            printf("[Server %d] Ho ricevuto un messaggio di TERMINAZIONE\n", getpid());
            break;
        }

        printf("[Server %d] Ho ricevuto una richiesta dal client %d con valore %d\n", getpid(), richiesta.pid, richiesta.data);

        sleep(1);
    }

    printf("[Server %d] Terminazione\n", getpid());
}