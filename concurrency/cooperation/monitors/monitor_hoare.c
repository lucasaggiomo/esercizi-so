#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "monitor_hoare.h"
#include "semaphore.h"

void init_monitor(struct Monitor* M, int num_var) {
    // alloca e inizializza il mutex del monitor
    M->id_mutex_sem = semget(IPC_PRIVATE, 1, IPC_CREAT | 0664);
    semctl(M->id_mutex_sem, 0, SETVAL, 1);

    // alloca e inizializza il semaforo per la coda urgente
    M->id_urgent_sem = semget(IPC_PRIVATE, 1, IPC_CREAT | 0664);
    semctl(M->id_urgent_sem, 0, SETVAL, 1);

    // alloca e inizializza num_var semafori per le variabili condition
    M->id_conds_sem = semget(IPC_PRIVATE, num_var, IPC_CREAT | 0664);
    for (int i = 0; i < num_var; i++) {
        semctl(M->id_conds_sem, i, SETVAL, 1);
    }

    // alloca un array di num_var interi per le variabili condition e un intero per il contatore della coda urgent
    M->id_shared = shmget(IPC_PRIVATE, (num_var + 1) * sizeof(int), IPC_CREAT | 0664);

    // esegue l'attach della memoria condivisa allocata
    M->cond_counts = shmat(M->id_shared, NULL, 0);
    M->urgent_count = M->cond_counts + num_var;

    // inizializza i contatori
    for (int i = 0; i < num_var; i++) {
        M->cond_counts[i] = 0;
    }
    *(M->urgent_count) = 0;

    // imposta il numero di variabili condition
    M->num_var_cond = num_var;
}

void enter_monitor(struct Monitor* M) {
    Wait_Sem(M->id_mutex_sem, 0);
}

void leave_monitor(struct Monitor* M) {
    // se c'è un processo nella coda urgent lo sveglio
    // (e quindi non segnalo il mutex del monitor, ovvero non faccio entrare nessun altro processo),
    // altrimenti segnalo il mutex del monitor, permettendo l'accesso al monitor
    if (*(M->urgent_count) > 0) {
        Signal_Sem(M->id_urgent_sem, 0);
    } else {
        Signal_Sem(M->id_mutex_sem, 0);
    }
}

void remove_monitor(struct Monitor* M) {
    // dealloca la memoria condivisa dei contatori per le variabili condition e per la coda urgent
    shmctl(M->id_shared, IPC_RMID, 0);

    // dealloca i semafori utilizzati per le code delle variabili condition
    semctl(M->id_conds_sem, 0, IPC_RMID, 0);

    // dealloca il semaforo utilizzato per la coda urgent
    semctl(M->id_urgent_sem, 0, IPC_RMID, 0);

    // dealloca il semaforo utilizzato per il mutex del monitor
    semctl(M->id_mutex_sem, 0, IPC_RMID, 0);
}

void wait_condition(struct Monitor* M, int var_cond_index) {
    // incrementa il contatore associato alla variabile condition
    M->cond_counts[var_cond_index]++;

    // prima di bloccarsi deve uscire dal monitor
    // la funzione leave_monitor si occupa di gestire la coda urgent e il mutex del monitor
    leave_monitor(M);

    // attendo la condizione
    Wait_Sem(M->id_conds_sem, var_cond_index);

    // decrementa il contatore associato alla variabile condition
    M->cond_counts[var_cond_index]--;
}

// signal and urgent wait
void signal_condition(struct Monitor* M, int var_cond_index) {
    // incrementa il contatore della coda urgent
    (*(M->urgent_count))++;

    if (M->cond_counts[var_cond_index] > 0) {
        Signal_Sem(M->id_conds_sem, var_cond_index);

        // anche nel caso in cui avvenisse una context switch in questo punto,
        // le eventuali wait_condition effettuano comunque una signal sul semaforo urgent

        Wait_Sem(M->id_urgent_sem, 0);
    }

    (*(M->urgent_count))--;
}

int queue_condition(struct Monitor* M, int id_var) {
    return M->cond_counts[id_var];
}