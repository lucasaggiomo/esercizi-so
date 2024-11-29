#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "header.h"

char parseCharFromString(const char* str) {
    if (str[1] != '\0') {
        printf("Errore nel parsing del carattere a partire dalla stirnga \"%s\"\n", str);
        exit(1);
    }
    return str[0];
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Errore nel parsing degli argomenti\n");
        exit(1);
    }
    char firstChar = parseCharFromString(argv[1]);
    char secondChar = parseCharFromString(argv[2]);

    printf("I caratteri inseriti sono: %c %c\n", firstChar, secondChar);

    // crea le code
    key_t sender_key = ftok(FTOK_PATH, firstChar);
    key_t receiver_key = ftok(FTOK_PATH, secondChar);

    int sender_id = msgget(sender_key, IPC_CREAT | 0644);
    if (sender_id < 0) {
        perror("Errore nella msgget del sender");
        exit(1);
    }

    int receiver_id = msgget(receiver_key, IPC_CREAT | 0644);
    if (receiver_id < 0) {
        perror("Errore nella msgget del receiver");
        exit(1);
    }

    /*  Si verifichi la correttezza del programma simulando 2 coppie di utenti che conversano,
        avviando 2 coppie di istanze del programma su 4 terminali diversi.*/

    // crea il mittente e il ricevente
    pid_t pid;
    for (int i = 0; i < 2; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella creazione di un processo");
            exit(1);
        } else if (pid == 0) {
            if (i == 0) {
                sender(receiver_id, sender_id);
            } else {
                receiver(receiver_id);
            }
            exit(0);
        }
    }

    // attende la terminazione dei processi creati
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    // deallocazione code
    msgctl(sender_id, IPC_RMID, NULL);
    msgctl(receiver_id, IPC_RMID, NULL);

    return 0;
}