#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        if (msgflg & IPC_NOWAIT && errno == ENOMSG) {
            return -1;
        }
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

void Execl(const char* pathname) {
    const char* name = strrchr(pathname, '/');
    if (!name) {
        name = pathname;
    } else {
        name++;
    }

    execl(pathname, name, NULL);
    perror("Errore nella execl");
    exit(1);
}

void Pthread_create(pthread_t* thread,
                    const pthread_attr_t* attr,
                    void* (*start_routine)(void*),
                    void* arg) {
    int ret = pthread_create(thread, attr, start_routine, arg);
    if (ret < 0) {
        perror("Errore nella pthread_create");
        exit(1);
    }
}

void Pthread_join(pthread_t thread, void** retval) {
    int ret = pthread_join(thread, retval);
    if (ret < 0) {
        perror("Errore nella pthread_join");
        // exit(1);
    }
}

void Pthread_cancel(pthread_t thread) {
    int ret = pthread_cancel(thread);
    if (ret < 0) {
        perror("Errore nella pthread_cancel");
        // exit(1);
    }
}

void* Malloc(size_t size) {
    void* p = malloc(size);
    if (!p) {
        perror("Errore nella malloc");
        exit(1);
    }
    return p;
}