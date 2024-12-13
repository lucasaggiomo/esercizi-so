#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "procedure.h"

int main() {
    key_t key_comandi_macchina = ftok(".", 'c');
    int id_comandi_macchina = msgget(key_comandi_macchina, 0);
    if (id_comandi_macchina < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_log_interfaccia = ftok(".", 't');
    int id_log_interfaccia = msgget(key_log_interfaccia, 0);
    if (id_log_interfaccia < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_rts_termostato = ftok(".", 'r');
    int id_rts_termostato = msgget(key_rts_termostato, IPC_CREAT | 0644);
    if (id_rts_termostato < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_ots_termostato = ftok(".", 'o');
    int id_ots_termostato = msgget(key_ots_termostato, IPC_CREAT | 0644);
    if (id_ots_termostato < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_comandi_termostato = ftok(".", 'T');
    int id_comandi_termostato = msgget(key_comandi_termostato, IPC_CREAT | 0644);
    if (id_comandi_termostato < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_feedback_termostato = ftok(".", 'K');
    int id_feedback_termostato = msgget(key_feedback_termostato, IPC_CREAT | 0644);
    if (id_feedback_termostato < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_rts_mixer = ftok(".", 'R');
    int id_rts_mixer = msgget(key_rts_mixer, IPC_CREAT | 0644);
    if (id_rts_mixer < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_ots_mixer = ftok(".", 'O');
    int id_ots_mixer = msgget(key_ots_mixer, IPC_CREAT | 0644);
    if (id_ots_mixer < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_comandi_mixer = ftok(".", 'M');
    int id_comandi_mixer = msgget(key_comandi_mixer, IPC_CREAT | 0644);
    if (id_comandi_mixer < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        controllore(id_comandi_macchina,
                    id_log_interfaccia,
                    id_rts_termostato,
                    id_ots_termostato,
                    id_comandi_termostato,
                    id_feedback_termostato,
                    id_rts_mixer,
                    id_ots_mixer,
                    id_comandi_mixer);
        exit(0);
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        termostato(id_rts_termostato,
                   id_ots_termostato,
                   id_comandi_termostato,
                   id_feedback_termostato);
        exit(0);
    }

    pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    } else if (pid == 0) {
        mixer(id_rts_mixer,
              id_ots_mixer,
              id_comandi_mixer);
        exit(0);
    }

    for (int i = 0; i < 3; i++) {
        int status;
        wait(&status);
        if (status != 0) {
            fprintf(stderr, "Attenzione! Un processo figlio ha terminato con status %d\n", status);
        }
    }

    msgctl(id_rts_termostato, IPC_RMID, NULL);
    msgctl(id_ots_termostato, IPC_RMID, NULL);
    msgctl(id_comandi_termostato, IPC_RMID, NULL);
    msgctl(id_feedback_termostato, IPC_RMID, NULL);

    msgctl(id_rts_mixer, IPC_RMID, NULL);
    msgctl(id_ots_mixer, IPC_RMID, NULL);
    msgctl(id_comandi_mixer, IPC_RMID, NULL);

    printf("[Macchina] Terminazione\n");

    return 0;
}