#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "wrapper.h"

int Msgget(key_t key, int msgflg) {
    int id = msgget(key, msgflg);
    if (id < 0) {
        perror("Errore nella msgget");
        exit(1);
    }
    return id;
}

int Msgctl(int msqid, int cmd, struct msqid_ds* buf) {
    int ret = msgctl(msqid, cmd, buf);
    if (ret < 0) {
        perror("Errore nella msgctl");
        // exit(1);
    }
    return ret;
}

void Msgsnd(int msqid, const void* msgp, size_t msgsz, int msgflg) {
    int ret = msgsnd(msqid, msgp, msgsz, msgflg);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }
}

ssize_t Msgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg) {
    int ret = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }
    return ret;
}

pid_t Fork() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        exit(1);
    }
    return pid;
}