#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#include "procedure.h"

char* formatted_msg(struct msg_generato msg) {
    static char buffer[256];     // preferisco questo (non thread-safe) per guadagnare in praticità, rispetto ad inserire il buffer negli argomenti
    snprintf(buffer, 256,
             "{ \"%s\"; [%d, %d]; %ld }",
             msg.str,
             msg.values[0], msg.values[1],
             msg.result);
    return buffer;
}

void genera(int id_dest) {
    struct msg_generato msg = { .type = 1 };
    for (int i = 0; i < 9; i++) {
        msg.str[i] = (rand() % 127-33) + 33;
    }
    msg.str[9] = '\0';
    msg.values[0] = rand() % 10;
    msg.values[1] = rand() % 10;
    msg.result = 0;

    printf("[Generatore %d] Prodotto %s\n", getpid(), formatted_msg(msg));

    Msgsnd(id_dest, &msg, SIZE_MSG_DATA, 0);
}

void filtra(int id_source, int id_dest) {
    struct msg_generato msg;
    Msgrcv(id_source, &msg, SIZE_MSG_DATA, 0, 0);

    char* x = strchr(msg.str, 'x');
    if (!x) {
        printf("[Filtro %d] Carattere 'x' NON TROVATO nel messaggio, non inoltro %s\n", getpid(), formatted_msg(msg));
        return;
    }

    printf("[Filtro %d] Carattere 'x' TROVATO nel messaggio, lo inoltro così %s\n", getpid(), formatted_msg(msg));
    Msgsnd(id_dest, &msg, SIZE_MSG_DATA, 0);
}

void checksumma(int id_source, int id_dest) {
    struct msg_generato msg;

    // termina se non c'è nessun messaggio (chiamare questa funzione quando si è certi che i messaggi sono stati inviati)
    int ret = Msgrcv(id_source, &msg, SIZE_MSG_DATA, 0, IPC_NOWAIT);
    if (ret < 0) {
        exit(0);
    }

    for (int i = 0; i < 9; i++) {
        msg.result += msg.str[i];
    }
    msg.result += msg.values[0] + msg.values[1];

    printf("[Checksum %d] Ho calcolato il risultato, inoltro il messaggio %s\n", getpid(), formatted_msg(msg));
    Msgsnd(id_dest, &msg, SIZE_MSG_DATA, 0);
}

void visualizza(int id_source) {
    struct msg_generato msg;

    // termina se non c'è nessun messaggio (chiamare questa funzione quando si è certi che i messaggi sono stati inviati)
    int ret = Msgrcv(id_source, &msg, SIZE_MSG_DATA, 0, IPC_NOWAIT);
    if (ret < 0) {
        exit(0);
    }

    printf("[Visualizzatore %d] Ho ricevuto il messaggio %s\n", getpid(), formatted_msg(msg));
}