#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "aeroporto.h"
#include "procedure.h"

#define NUM_DISPLAY 2

int main() {
    int id_avvisi = msgget(IPC_PRIVATE, IPC_CREAT | 0644);
    if (id_avvisi < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    int id_aeroporto = shmget(IPC_PRIVATE, sizeof(struct aeroporto), IPC_CREAT | 0644);
    if (id_aeroporto < 0) {
        perror("Errore nella shmget");
        exit(1);
    }

    struct aeroporto* a = shmat(id_aeroporto, NULL, 0);
    if (a == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    int id_sem_printf = semget(IPC_PRIVATE, 1, IPC_CREAT | 0644);
    if (id_sem_printf < 0) {
        perror("Errore nella semget");
        exit(1);
    }
    semctl(id_sem_printf, PRINTF_SEM, SETVAL, 1);

    set_id_printf_sem(id_sem_printf);

    init_aeroporto(a);

    pid_t pid;

    for (int i = 0; i < NUM_GATE; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            gate(id_avvisi, i + 1);
            exit(0);
        }
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        aggiornatore(id_avvisi, a);
        exit(0);
    }

    for (int i = 0; i < NUM_DISPLAY; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore nella fork");
            exit(1);
        } else if (pid == 0) {
            display(a);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_DISPLAY + NUM_GATE; i++) {
        wait(NULL);
    }
    wait(NULL);

    msgctl(id_avvisi, IPC_RMID, NULL);

    destroy_aeroporto(a);
    shmctl(id_aeroporto, IPC_RMID, NULL);

    semctl(id_sem_printf, 0, IPC_RMID, NULL);

    return 0;
}