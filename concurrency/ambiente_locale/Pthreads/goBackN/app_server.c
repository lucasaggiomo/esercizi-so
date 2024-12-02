#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "app_server.h"

#define MAX_LENGTH 256

int get_message(int id_app_dati, char* message) {
    printf("[App Server] Attendo il messaggio...\n");

    int len = 0;
    // attende la ricezione dei messaggi inviati dal client in ordine di invio (li riceve dal livello trasporto server)
    for (int i = 0; i < NUM_MESSAGGI; i++) {
        int ret;

        struct msg_app msg;
        ret = msgrcv(id_app_dati, &msg, SIZE_MSG_APP, 0, 0);
        if (ret < 0) {
            perror("[App Server] Errore nella msgrcv del messaggio");
            exit(1);
        }

        printf("[App Server] Ho ricevuto il carattere \'%c\'\n", msg.character);

        message[len] = msg.character;
        len++;

        if (len == MAX_LENGTH) {
            printf("[App Server] Attenzione! Il messaggio ricevuto ha raggiunto la lunghezza massima! Non verranno accettati ulteriori caratteri\n");
            break;
        }
    }
    message[len] = '\0';
    return len;
}

void app_server(int id_app_dati) {
    printf("[App Server] In esecuzione\n");

    char message[MAX_LENGTH + 1];
    int len = get_message(id_app_dati, message);

    printf("[App Server] Il messaggio ricostruito in ordine è \"%s\"\n", message);

    int fd = open("./output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fd < 0) {
        perror("[App Server] Errore nell'apertura del file output.txt");
        exit(1);
    }

    int ret = write(fd, message, len);
    if (ret < 0) {
        perror("[App Server] Errore nella scrittura sul file output.txt");
    }

    ret = close(fd);
    if (ret < 0) {
        perror("[App Server] Errore nella chiusura del file output.txt");
        exit(1);
    }

    printf("[App Server] Terminazione\n");
}