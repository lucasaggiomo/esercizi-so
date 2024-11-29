#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "header.h"

void sender(int id_queue_receiver, int id_queue_sender) {
    /*  Il processo figlio mittente eseguirà un loop in cui ad ogni iterazione si mette in attesa
        di una stringa dall'utente dallo standard input, ed invia un messaggio con la stringa
        sulla prima coda di messaggi condivisa.Quando l'utente inserisce "exit" seguito da un carattere di invio,
        il programma deve inviare un messaggio con una stringa "exit" sulla seconda coda e terminare. */
    struct msg mess;
    int ret;

    while (1) {
        printf("Inserisci il messaggio da inviare [max 20 caratteri]\n");
        scanf("%s", mess.message);

        mess.msgType = TYPE;

        // termina se riceve exit
        if (strcmp(mess.message, EXIT_STRING) == 0) {

            ret = msgsnd(id_queue_receiver, &mess, SIZE_MSG, 0);

            if (ret < 0) {
                // perror("[SENDER] Errore nella msgsnd di exit");
                exit(1);
            }

            printf("[SENDER] inviato: %s\n", mess.message);

            exit(0);
        }

        // se sono qui non è arrivato exit

        ret = msgsnd(id_queue_sender, &mess, SIZE_MSG, 0);

        if (ret < 0) {
            // perror("[SENDER] Errore nella msgsnd");
            exit(1);
        }

        printf("[SENDER] inviato: %s\n", mess.message);
    }
}

void receiver(int id_queue_receiver) {
    /*  Il processo figlio ricevente eseguirà un loop in cui ad ogni iterazione si metterà in attesa
        di un messaggio dalla seconda coda, e stamperà sullo standard output la stringa ricevuta.
        In caso di ricezione di un messaggio con la stringa "exit", il processo dovrà terminare.*/

    struct msg mess;
    int ret;

    while (1) {
        ret = msgrcv(id_queue_receiver, &mess, SIZE_MSG, 0, 0);

        if (ret < 0) {
            // perror("[RECEIVER] Errore nella ricezione del messaggio");
            exit(1);
        }

        printf("[RECEIVER] ricevuto: %s\n", mess.message);

        if (strcmp(mess.message, EXIT_STRING) == 0) {
            exit(0);
        }
    }
}
