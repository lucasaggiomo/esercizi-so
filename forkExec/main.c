#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {
    // printf("Inizio esecuzione del processo %d con padre %d e gruppo %d\n\n", getpid(), getppid(), getpgrp());
    pid_t pid = fork();
    if (pid < 0) {
        perror("Errore nella creazione del processo");
        exit(1);
    } else if (pid == 0) {
        // codice del figlio
        printf("Sono il figlio %d con padre %d e gruppo %d\n\n", getpid(), getppid(), getpgrp());

        char* myArgv[] = {"ls", "-l", NULL};

        int output = execvp("ls", myArgv);
        if (output == -1)
            perror("Errore nella funzione exec");

        printf("Questo codice non viene eseguito se exec ha avuto esito positivo");
        exit(1);
    } else if (pid > 0) {
        int status;
        int waited_pid = wait(&status);
        printf("Sono il padre %d, ed ho fatto un figlio con pid %d\nHo aspettato che morisse il figlio con pid %d, che è morto con status %d\n", getpid(), pid, waited_pid, WEXITSTATUS(status));
    }
    return 0;
}