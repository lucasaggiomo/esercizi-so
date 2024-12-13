#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "procedure.h"

#define CLIENT_STR "CLIENT"

#define NUM_CLIENT 3
#define NUM_MESSAGGI_PER_CLIENT 3
#define TOTALE_MESSAGGI (NUM_CLIENT * NUM_MESSAGGI_PER_CLIENT)

#define VALUE_CORRPUTION_CHANCE .1f
#define STRING_CORRPUTION_CHANCE .2f

int main() {
    int id_iniettore = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    if (id_iniettore < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    int id_server = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    if (id_server < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    pid_t pid;

    for (int i = 1; i <= NUM_CLIENT; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            char buffer[21];
            snprintf(buffer, 21, "%s%d", CLIENT_STR, i);

            client(id_iniettore,
                   buffer,
                   NUM_MESSAGGI_PER_CLIENT);

            exit(0);
        }
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        iniettore(id_iniettore,
                  id_server,
                  VALUE_CORRPUTION_CHANCE,
                  STRING_CORRPUTION_CHANCE,
                  TOTALE_MESSAGGI);
        exit(0);
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        server(id_server, TOTALE_MESSAGGI);
        exit(0);
    }

    for (int i = 0; i < NUM_CLIENT + 2; i++) {
        int status;
        wait(&status);
        if (status != 0) {
            fprintf(stderr, "Attenzione! Un processo figlio è morto con status %d\n", status);
        }
    }

    msgctl(id_iniettore, IPC_RMID, NULL);
    msgctl(id_server, IPC_RMID, NULL);

    return 0;
}