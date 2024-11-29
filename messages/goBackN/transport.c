#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "transport.h"

int send_maybe(int id_coda, void* msg, size_t size_msg, int msgflg, float chance) {
    int todo = toDoOrNotToDo(chance);

    sleep(1);

    if (!todo)
        return 0;

    todo = msgsnd(id_coda, msg, size_msg, msgflg);
    if (todo < 0) {
        perror("Errore nella send maybe");
        exit(1);
    }
    return 1;
}

int toDoOrNotToDo(float chance) {
    if (chance < 0.0f || chance > 1.0f) {
        fprintf(stderr, "Errore: la probabilità deve essere tra 0.0 e 1.0\n");
        exit(1);
    }

    float random = (float)rand() / RAND_MAX;

    return random < chance ? 1 : 0;
}