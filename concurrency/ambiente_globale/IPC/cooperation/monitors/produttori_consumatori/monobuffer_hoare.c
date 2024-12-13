#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../monitor_hoare.h"
#include "../semaphore.h"

#define CV_PRODUTTORE 0
#define CV_CONSUMATORE 1

#define NUM_PRODUTTORI 6
#define NUM_CONSUMATORI 4

#define NUM_PRODUZIONI 2
#define NUM_CONSUMI 3

#define false 0
#define true 1

struct buffer {
    int buffer;

    int buffer_pieno;

    struct Monitor m;
};

void produzione(struct buffer* b, int value, int num) {
    printf("[%d | PRODUTTORE %d]\tProvo ad entrare nel monitor\n", getpid(), num);

    enter_monitor(&(b->m));

    printf("[%d | PRODUTTORE %d]\tSono entrato nel monitor\n", getpid(), num);

    if (b->buffer_pieno) {
        wait_condition(&(b->m), CV_PRODUTTORE);
        printf("[%d | PRODUTTORE %d]\tSono stato svegliato dalla coda della wait_condition\n", getpid(), num);
    }

    printf("[%d | PRODUTTORE %d]\tInizio la produzione\n", getpid(), num);

    sleep(1);

    b->buffer = value;
    b->buffer_pieno = true;

    printf("[%d | PRODUTTORE %d]\tHo prodotto il valore %d, ora segnalo i consumatori\n", getpid(), num, value);

    signal_condition(&(b->m), CV_CONSUMATORE);

    printf("[%d | PRODUTTORE %d]\tSono tornato dalla coda urgent, provo ad uscire dal monitor\n", getpid(), num);

    leave_monitor(&(b->m));

    printf("[%d | PRODUTTORE %d]\tSono uscito dal monitor\n", getpid(), num);
}

int consumo(struct buffer* b, int num) {
    int value;

    printf("[%d | CONSUMATORE %d]\tProvo ad entrare nel monitor\n", getpid(), num);

    enter_monitor(&(b->m));

    printf("[%d | CONSUMATORE %d]\tSono entrato nel monitor\n", getpid(), num);

    if (!(b->buffer_pieno)) {
        wait_condition(&(b->m), CV_CONSUMATORE);
        printf("[%d | CONSUMATORE %d]\tSono stato svegliato dalla coda della wait_condition\n", getpid(), num);
    }

    printf("[%d | CONSUMATORE %d]\tInizio il consumo\n", getpid(), num);

    sleep(1);

    value = b->buffer;
    b->buffer_pieno = false;

    printf("[%d | CONSUMATORE %d]\tHo letto il valore %d\n", getpid(), num, value);

    signal_condition(&(b->m), CV_PRODUTTORE);

    printf("[%d | CONSUMATORE %d]\tSono tornato dalla coda urgent, provo ad uscire dal monitor\n", getpid(), num);

    leave_monitor(&(b->m));

    printf("[%d | CONSUMATORE %d]\tSono uscito dal monitor\n", getpid(), num);

    return value;
}

int main() {
    struct buffer* buff;

    // alloca un buffer condiviso
    int id_buffer = shmget(IPC_PRIVATE, sizeof(*buff), IPC_CREAT | 0644);

    if (id_buffer < 0) {
        perror("Errore nella creazione del buffer condiviso\n");
        exit(1);
    }

    // eseguo l'attach del buffer
    buff = shmat(id_buffer, NULL, 0);

    if (buff == (void*)-1) {
        perror("Errore nell'attach del buffer condiviso\n");
        exit(1);
    }

    // inizializzo il buffer
    buff->buffer = 0;
    buff->buffer_pieno = false;
    init_monitor(&(buff->m), 2);

    printf("Inizio a creare i produttori e i consumatori\n");

    // creo i produttori e i consumatori
    for (int i = 0; i < NUM_PRODUTTORI; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Errore nella creazione del produttore numero %d: %s\n", i + 1, strerror(errno));
            exit(1);
        } else if (pid == 0) {
            srand(getpid());

            for (int j = 0; j < NUM_PRODUZIONI; j++) {
                produzione(buff, rand() % 16, j);
            }

            exit(0);
        }
    }
    for (int i = 0; i < NUM_CONSUMATORI; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Errore nella creazione del consumatore numero %d: %s\n", i + 1, strerror(errno));
            exit(1);
        } else if (pid == 0) {
            for (int j = 0; j < NUM_CONSUMI; j++) {
                consumo(buff, j);
            }

            exit(0);
        }
    }

    // aspetto che muoiano i figli
    for (int i = 0; i < NUM_PRODUTTORI + NUM_CONSUMATORI; i++) {
        wait(NULL);
    }

    printf("Dealloco la memoria condivisa\n");

    // dealloco il buffer
    remove_monitor(&(buff->m));
    shmctl(id_buffer, IPC_RMID, 0);

    return 0;
}