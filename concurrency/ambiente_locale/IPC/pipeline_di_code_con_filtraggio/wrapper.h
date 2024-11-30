#ifndef WRAPPER_H
#define WRAPPER_H

#include <sys/msg.h>
#include <sys/types.h>

int Msgget(key_t key, int msgflg);
int Msgctl(int msqid, int cmd, struct msqid_ds* buf);
void Msgsnd(int msqid, const void* msgp, size_t msgsz, int msgflg);
ssize_t Msgrcv(int msqid, void* msgp, size_t msgsz, long msgtyp, int msgflg);

pid_t Fork();

#endif