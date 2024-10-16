#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../../utils/semaphore.h"

#define SYNC 0     // indice semaforo per la sincronizzazione tra lettori e scrittori
#define MUTEX_L 1  // indice semaforo per la mutua esclusione della variabile num_lettori per i lettori
#define MUTEX_S 2  // indice semaforo per la mutua esclusione della variabile num_scrittori per gli scrittori
#define MUTEX 3    // indice semaforo per la mutua esclusione della variabile data per gli scrittori

#define NUM_LETTORI 10
#define NUM_SCRITTORI 3

#define NUM_OPERAZIONI_LETTURE 1
#define NUM_OPERAZIONI_SCRITTURE 3

struct shared {
    int num_lettori;
    int num_scrittori;
    int data;
};

void inizio_lettura(struct shared* shm, int semid) {
    // decrementa il semaforo MUTEX_L. Se è < 0 (cioè c'è un lettore che sta incrementando num_lettori) si blocca
    Wait_Sem(semid, MUTEX_L);

    shm->num_lettori++;

    // se c'è un solo lettore, quindi SOLO per il primo lettore,
    // decrementa il semaforo SYNC. Se è < 0 (cioè uno scrittore sta scrivendo) si blocca
    if (shm->num_lettori == 1)
        Wait_Sem(semid, SYNC);

    // incrementa il semaforo MUTEX_L. Se è <= 0 (cioè c'è un lettore bloccato che attende di incrementare num_lettori) sveglia un lettore bloccato
    Signal_Sem(semid, MUTEX_L);
}

void fine_lettura(struct shared* shm, int semid) {
    // decrementa il semaforo MUTEX_L. Se è < 0 (cioè c'è un lettore che sta decrementando num_lettori) si blocca
    Wait_Sem(semid, MUTEX_L);

    shm->num_lettori--;

    // se non ci sono più lettori, quindi SOLO per l'ultimo lettore,
    // incrementa il semaforo SYNC. Se è <= 0 (cioè uno scrittore è bloccato) sveglia un processo (scrittore) in coda
    if (shm->num_lettori == 0)
        Signal_Sem(semid, SYNC);

    // incrementa il semaforo MUTEX_L. Se è <= 0 (cioè c'è un lettore bloccato che attende di decrementare num_lettori) sveglia un lettore bloccato
    Signal_Sem(semid, MUTEX_L);
}

void lettore(struct shared* shm, int semid) {
    inizio_lettura(shm, semid);

    sleep(2);
    printf("Lettura [%d]:\t%d\n", getpid(), shm->data);

    fine_lettura(shm, semid);
}

void inizio_scrittrura(struct shared* shm, int semid) {
    // decrementa il semaforo MUTEX_S. Se è < 0 (cioè c'è uno scrittore che sta incrementando num_scrittori) si blocca
    Wait_Sem(semid, MUTEX_S);

    shm->num_scrittori++;

    // se c'è un solo scrittore, quindi SOLO per il primo scrittore,
    // decrementa il semaforo SYNC. Se è < 0 (cioè un lettore sta leggendo) si blocca
    if (shm->num_scrittori == 1)
        Wait_Sem(semid, SYNC);

    // incrementa il semaforo MUTEX_S. Se è <= 0 (cioè c'è uno scrittore bloccato che attende di incrementare num_scrittori) sveglia uno scrittore bloccato
    Signal_Sem(semid, MUTEX_S);

    // decrementa il semaforo MUTEX. Se è < 0 (cioè c'è uno scrittore che sta scrivendo sul buffer) si blocca
    Wait_Sem(semid, MUTEX);
}

void fine_scrittura(struct shared* shm, int semid) {
    // incrementa il semaforo MUTEX. Se è <= 0 (cioè c'è uno scrittore bloccato che attende di scrivere sul buffer) lo sveglia
    Signal_Sem(semid, MUTEX);

    // decrementa il semaforo MUTEX_S. Se è < 0 (cioè c'è uno scrittore che sta decrementando num_scrittori) si blocca
    Wait_Sem(semid, MUTEX_S);

    shm->num_scrittori--;

    // se non ci sono più scrittori, quindi SOLO per l'ultimo scrittore,
    // incrementa il semaforo SYNC. Se è <= 0 (cioè un lettore è bloccato) sveglia un processo (lettore) in coda
    if (shm->num_scrittori == 0)
        Signal_Sem(semid, SYNC);

    // incrementa il semaforo MUTEX_S. Se è <= 0 (cioè c'è uno scrittore bloccato che attende di decrementare num_scrittori) sveglia uno scrittore bloccato
    Signal_Sem(semid, MUTEX_S);
}

void scrittore(struct shared* shm, int semid) {
    inizio_scrittrura(shm, semid);

    shm->data = rand() % 16;
    sleep(1);
    printf("Scrittura [%d]:\t%d\n", getpid(), shm->data);

    fine_scrittura(shm, semid);
}

void fork_lettore(struct shared* shm, int semid, int letture, unsigned int sleep_seconds) {
    // creo il lettore (che legge shm)
    int pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella creazione del lettore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio

        // simulo un delay per far arrivare i lettori dilazionati nel tempo
        sleep(sleep_seconds);
        printf("ARRIVA LETTORE\t\t[%d]\n", getpid());
        for (int i = 0; i < letture; i++) {
            lettore(shm, semid);
        }

        // termina l'esecuzione
        exit(EXIT_SUCCESS);
    }
}

void fork_scrittore(struct shared* shm, int semid, int scritture, unsigned int sleep_seconds) {
    // creo lo scrittore (che scrive su shm)
    int pid = fork();

    if (pid < 0) {
        // errore
        perror("Errore nella creazione dello scrittore");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
        // processo figlio
        srand(getpid());

        // simulo un delay per far arrivare gli scrittori dilazionati nel tempo
        sleep(sleep_seconds);
        printf("ARRIVA SCRITTORE\t[%d]\n", getpid());
        for (int i = 0; i < scritture; i++) {
            scrittore(shm, semid);
        }

        // termina l'esecuzione
        exit(EXIT_SUCCESS);
    }
}

int main() {
    srand(time(NULL));

    // creo due semafori (array di 4 semafori)
    int semid = semget(IPC_PRIVATE, 4, IPC_CREAT | 0644);

    // gestisco eventuali errori
    if (semid < 0) {
        perror("Errore nella creazione dei semafori");
        exit(EXIT_FAILURE);
    }

    // inizializzo i semafori
    int result = semctl(semid, SYNC, SETVAL, 1);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo SYNC");
        exit(EXIT_FAILURE);
    }

    result = semctl(semid, MUTEX_L, SETVAL, 1);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo MUTEX_L");
        exit(EXIT_FAILURE);
    }

    result = semctl(semid, MUTEX_S, SETVAL, 1);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo MUTEX_S");
        exit(EXIT_FAILURE);
    }

    result = semctl(semid, MUTEX, SETVAL, 1);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nell'inizializzazione del valore del semaforo MUTEX");
        exit(EXIT_FAILURE);
    }

    // creo la memoria condivisa
    int ds_shm = shmget(IPC_PRIVATE, sizeof(struct shared), IPC_CREAT | 0644);

    if (ds_shm < 0) {
        perror("Errore nella creazione della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // collego ("attach") il segmento di memoria allo spazio di indirizzamento del chiamante
    struct shared* shm = shmat(ds_shm, NULL, 0);

    if (shm == (void*)-1) {
        perror("Errore nel colllegamento della memoria condivisa");
        exit(EXIT_FAILURE);
    }

    // inizializzo num_lettori, num_scrittori e data
    shm->num_lettori = 0;
    shm->num_scrittori = 0;
    shm->data = 0;

    // creo scrittori e lettori
    for (int i = 0; i < NUM_SCRITTORI; i++) {
        fork_scrittore(shm, semid, NUM_OPERAZIONI_SCRITTURE, i/2);
    }

    for (int i = 0; i < NUM_LETTORI; i++) {
        fork_lettore(shm, semid, NUM_OPERAZIONI_LETTURE, i/2);
    }

    // il padre attende la terminazione dei lettori e degli scrittori
    for (int i = 0; i < NUM_LETTORI + NUM_SCRITTORI; i++) {
        wait(NULL);
        wait(NULL);
    }

    // elimino la risorsa dell'array di semafori
    result = semctl(semid, 0, IPC_RMID);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nella cancellazione dei semafori");
        exit(EXIT_FAILURE);
    }

    // elimino la risorsa condivisa
    result = shmctl(ds_shm, IPC_RMID, NULL);

    // gestisco eventuali errori
    if (result < 0) {
        perror("Errore nella cancellazione della risorsa condivisa");
        exit(EXIT_FAILURE);
    }

    return 0;
}