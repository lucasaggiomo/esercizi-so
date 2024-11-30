#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

#include "messaggi.h"
#include "procedure.h"
#include "wrapper.h"

#define NUM_GENERATORI 3

#define NUM_GENERAZIONI 2
#define NUM_FILTRAGGI 6
#define NUM_CHECKSUMS 6
#define NUM_VISUALIZATIONS 6

int main() {
    int id_generati = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    int id_filtrati = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    int id_checksummati = Msgget(IPC_PRIVATE, IPC_CREAT | 0644);

    pid_t pid;

    for (int i = 0; i < NUM_GENERATORI; i++) {
        pid = Fork();
        if (pid > 0)
            continue;

        srand(getpid());
        for (int j = 0; j < NUM_GENERAZIONI; j++) {
            genera(id_generati);
        }
        exit(0);
    }

    pid = Fork();
    if (pid == 0) {
        for (int i = 0; i < NUM_FILTRAGGI; i++) {
            filtra(id_generati, id_filtrati);
        }
        exit(0);
    }

    // checksum e visualizzatore attendono 3 secondi per essere certi che tutti i messaggi sono stati inviati
    // questo perché il filtro potrebbe non mandare tutti e 6 i messaggi, portando i processi checksum e visualizzatori a bloccarsi

    pid = Fork();
    if (pid == 0) {
        sleep(3);
        for (int i = 0; i < NUM_CHECKSUMS; i++) {
            checksumma(id_filtrati, id_checksummati);
        }
        exit(0);
    }

    pid = Fork();
    if (pid == 0) {
        sleep(3);
        for (int i = 0; i < NUM_VISUALIZATIONS; i++) {
            visualizza(id_checksummati);
        }
        exit(0);
    }

    for (int i = 0; i < NUM_GENERATORI + 3; i++) {
        wait(NULL);
    }

    Msgctl(id_generati, IPC_RMID, NULL);
    Msgctl(id_filtrati, IPC_RMID, NULL);
    Msgctl(id_checksummati, IPC_RMID, NULL);
}