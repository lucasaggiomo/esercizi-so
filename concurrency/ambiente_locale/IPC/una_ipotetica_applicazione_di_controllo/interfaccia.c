#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"

#define COMANDO_AVVIO_PREPARAZIONE "avvio"

int main() {
    int ret;

    key_t key_comandi_macchina = ftok(".", 'c');
    int id_comandi_macchina = msgget(key_comandi_macchina, 0);
    if (id_comandi_macchina < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_log = ftok(".", 't');
    int id_log = msgget(key_log, 0);
    if (id_log < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    // attende un input dall'utente
    char buffer[255];

    while (1) {
        printf("[Interfaccia] Inserisci il comando di avvio quando sei pronto (ovvero \"%s\")\n", COMANDO_AVVIO_PREPARAZIONE);
        ret = scanf("%s", buffer);
        if (ret < 0) {
            perror("Errore nella scanf");
            exit(1);
        }
        if (strcmp(buffer, COMANDO_AVVIO_PREPARAZIONE) != 0) {
            printf("Il comando \"%s\" non esiste\n", buffer);
        } else {
            break;
        }
    };

    // invia il comando di AVVIO PREPARAZIONE
    printf("[Interfaccia] Invio un comando di AVVIO PREPARAZIONE alla macchina\n");
    struct msg_comando comando = {
        .type = AVVIO_PREPARAZIONE
    };
    ret = msgsnd(id_comandi_macchina, &comando, SIZE_MSG_COMANDO, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }

    // si mette in ascolto sul log della macchina e scrive tutto in log.txt
    int fd_log = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_log < 0) {
        perror("Errore nell'apertura del file di log");
        exit(1);
    }
    while (1) {
        struct msg_log log;
        ret = msgrcv(id_log, &log, SIZE_MSG_LOG, 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        printf("[Interfaccia] Log ricevuto: %s\n", log.message);
        ret = write(fd_log, log.message, strlen(log.message));
        if (ret < 0) {
            perror("Errore nella write");
            exit(1);
        }

        if (log.type == TERMINAZIONE) {
            printf("[Interfaccia] Ho ricevuto un messaggio di TERMINAZIONE\n");
            break;
        }
    }
    close(fd_log);

    printf("[Interfaccia] Terminazione\n");

    return 0;
}