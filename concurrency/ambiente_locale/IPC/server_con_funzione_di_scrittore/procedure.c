#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"
#include "semaphore.h"

#include "procedure.h"

// Funzione setter per modificare la variabile statica
static int id_sem_printf;
void set_id_printf_sem(int value) {
    id_sem_printf = value;
}
int printf_sem(const char* format, ...) {
    va_list args;               // Dichiarazione della lista di argomenti variabili
    va_start(args, format);     // Inizializzazione della lista di argomenti con il formato

    Wait_Sem(id_sem_printf, PRINTF_SEM);
    // Chiamata a vprintf per passare gli argomenti
    int result = vprintf(format, args);
    Signal_Sem(id_sem_printf, PRINTF_SEM);

    va_end(args);     // Pulizia della lista di argomenti variabili

    return result;     // Ritorna il valore restituito da vprintf
}

/*
Gli avvisi dovranno contenere il numero del
porto di imbarco (da 1 a 4), il codice del volo (un intero casuale tra 0
e 99), e l' indicazione del tipo di volo (un intero casuale pari a 0 se
il volo è in arrivo, 1 se in partenza). Ogni processo **Gate** dovrà
generare 3 avvisi, attendendo un tempo casuale (compreso tra 1 e 3
secondi) tra un avviso e l'altro, ed infine terminare.
*/
void gate(int id_avvisi, int num_gate) {
    srand(getpid());

    sleep(num_gate);

    for (int i = 0; i < 3; i++) {
        struct msg_avviso avviso = {
            .type = 1,
            .numero_gate = num_gate,
            .gate = {
                .codice = rand() % 100,
                .stato = (enum stato)(rand() % 2),
            }
        };

        printf_sem("[Gate %d] Invio avviso del gate [%d] [codice: %d, stato: %d]\n", getpid(), avviso.numero_gate, avviso.gate.codice, avviso.gate.stato);
        int ret = msgsnd(id_avvisi, &avviso, SIZEOF_MSG(struct msg_avviso), 0);
        if (ret < 0) {
            perror("Errore nella msgsnd");
            exit(1);
        }

        sleep(1);
    }
}

/*
    Al ricevere di un avviso, il processo **Aggiornatore** dovrà scrivere il
    codice del volo all'interno della seguente struttura dati, che dovrà
    contenere, per ogni porto di imbarco, il codice e il tipo del volo
    previsto sul porto di imbarco. La struttura va allocata in una **memoria
    condivisa UNIX**.
*/

void aggiornatore(int id_avvisi, struct aeroporto* a) {
    for (int i = 0; i < 4; i++) {
        struct msg_avviso avviso;
        int ret = msgrcv(id_avvisi, &avviso, SIZEOF_MSG(struct msg_avviso), 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        printf_sem("[Aggiornatore] Ho ricevuto un avviso sul gate [%d] [codice: %d, stato: %d], lo scrivo sui display\n", avviso.numero_gate, avviso.gate.codice, avviso.gate.stato);
        scrivi_gate(a, avviso.numero_gate - 1, &avviso.gate);
    }
}

/*
    una coppia di processi Display dovrà periodicamente (una volta ogni secondo, per un totale di 10 iterazioni)
    leggere la struttura dati condivisa, e stampare a video lo stato dei porti di imbarco
*/

void display(struct aeroporto* a) {
    struct gate gates[NUM_GATE];
    for (int i = 0; i < 10; i++) {
        leggi_aeroporto(a, gates);

        Wait_Sem(id_sem_printf, PRINTF_SEM);
        printf("\n");
        for (int k = 0; k < NUM_GATE; k++) {
            printf("[Display %d] Gate %d: [codice: %d, stato: %d]\n", getpid(), i + 1, gates[k].codice, gates[k].stato);
        }
        printf("\n");
        Signal_Sem(id_sem_printf, PRINTF_SEM);

        sleep(1);
    }
}