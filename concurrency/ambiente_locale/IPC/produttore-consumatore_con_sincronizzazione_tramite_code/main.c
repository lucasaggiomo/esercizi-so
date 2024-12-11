#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

int main() {
    key_t key_shm = ftok(".", 's');
    int id_shm = shmget(key_shm, sizeof(buffer_t), IPC_CREAT | 0644);
    if (id_shm < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    key_t key_consumazioni = ftok(".", 'c');
    int id_consumazioni = msgget(key_consumazioni, IPC_CREAT | 0644);
    if (id_consumazioni < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_produzioni = ftok(".", 'p');
    int id_produzioni = msgget(key_produzioni, IPC_CREAT | 0644);
    if (id_produzioni < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./produttore", "produttore", NULL);
        perror("Errore nella execl");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        execl("./consumatore", "consumatore", NULL);
        perror("Errore nella execl");
        exit(1);
    }

    wait(NULL);
    wait(NULL);

    msgctl(id_produzioni, IPC_RMID, NULL);
    msgctl(id_consumazioni, IPC_RMID, NULL);
    shmctl(id_shm, IPC_RMID, NULL);

    return 0;
}