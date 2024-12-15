#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "buffer.h"

#include "messaggi.h"
#include "procedure.h"

struct buffer_arr* allocate_sem_and_buffer(key_t key_buffer, int* id_buffer, int length);
void handle_command(int id_rts_comandi, int id_ots_comandi, int id_comandi, int id_risposte);

int main() {
    srand(getpid());

    key_t key_comandi = ftok(".", 'c');
    int id_comandi = msgget(key_comandi, 0);
    if (id_comandi < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_risposte = ftok(".", 'p');
    int id_risposte = msgget(key_risposte, 0);
    if (id_risposte < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_rts_comandi = ftok(".", 'r');
    int id_rts_comandi = msgget(key_rts_comandi, 0);
    if (id_rts_comandi < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    key_t key_ots_comandi = ftok(".", 'r');
    int id_ots_comandi = msgget(key_ots_comandi, 0);
    if (id_ots_comandi < 0) {
        perror("Errore nella msgget");
        exit(1);
    }

    handle_command(id_rts_comandi, id_ots_comandi, id_comandi, id_risposte);

    printf("[Server] Terminazione\n");

    return 0;
}

struct buffer_arr* allocate_sem_and_buffer(key_t key_buffer, int* id_buffer, int length) {
    int id_sem = semget(IPC_PRIVATE, 2, IPC_CREAT | 0644);
    if (id_sem < 0) {
        perror("Errore nella semget");
        exit(1);
    }

    semctl(id_sem, SPAZIO_DISP, SETVAL, length);
    semctl(id_sem, MESSAGGIO_DISP, SETVAL, 0);

    *id_buffer = shmget(key_buffer, sizeof(struct buffer_arr), IPC_CREAT | 0644);
    if (*id_buffer < 0) {
        perror("Erorre nella shmget");
        exit(1);
    }

    struct buffer_arr* arr = shmat(*id_buffer, NULL, 0);
    if (arr == (void*)-1) {
        perror("Errore nella shmat");
        exit(1);
    }

    init_buffer_arr(arr, length, id_sem);

    return arr;
}

void handle_command(int id_rts_comandi, int id_ots_comandi, int id_comandi, int id_risposte) {
    int ret;
    struct msg_comando comando;
    key_t key_shm;
    int number_of_shm = 0;
    int id_shm = -1;
    struct buffer_arr* shm = NULL;

    while (1) {
        // si mette in attesa di un comando (da una send sincrona (rts - ots))
        receive_sincrona(id_comandi, id_rts_comandi, id_ots_comandi, &comando, SIZEOF_MSG(comando));

        switch (comando.type) {
            case RICHIESTA_SHM:
                if (shm) {
                    fprintf(stderr, "[Server] Comando inaspettato! Ho ricevuto un comando di RICHIESTA ma non ho mai ricevuto un comando TERMINA SHM in precedenza!\n");
                    exit(1);
                }

                // alloca una memoria condivisa
                key_shm = ftok(".", number_of_shm++);
                int length = rand() % 6 + 5;     // [5, 10]

                printf("[Server] Ricevuto un comando di RICHIESTA, alloco una coda di %d elementi\n", length);

                shm = allocate_sem_and_buffer(key_shm, &id_shm, length);

                printf("[Server] Invio un messaggio di risposta con chiave %d al client\n", key_shm);

                // invia una risposta al client
                struct msg_risposta risposta = {
                    .type = 1,
                    .key_shm = key_shm,
                    .length = length
                };
                ret = msgsnd(id_risposte, &risposta, SIZEOF_MSG(struct msg_risposta), 0);
                if (ret < 0) {
                    perror("Errore nella msgsnd");
                    exit(1);
                }

                // produce sul vettore di buffer una sequenza di coppie di valori (scelti casualmente tra 0 e 3)
                for (int i = 0; i < length; i++) {
                    struct buffer arr = {
                        .val1 = rand() % 4,
                        .val2 = rand() % 4
                    };

                    printf("[Server] Produco la coppia [%d, %d]\n", arr.val1, arr.val2);
                    produci(shm, &arr);
                }
                printf("[Server] Ho terminato la produzione, attendo un messaggio di terminazione]\n");

                break;

            case TERMINA_SHM:
                if (!shm) {
                    fprintf(stderr, "[Server] Comando inaspettato! Ho ricevuto un comando TERMINA SHM ma non ho mai ricevuto una RICHIESTA in precedenza!\n");
                    exit(1);
                }

                // dealloca semafori e memoria condivisa
                semctl(shm->id_sem, 0, IPC_RMID, NULL);

                shmctl(id_shm, IPC_RMID, NULL);

                id_shm = -1;
                shm = NULL;

                printf("[Server] Comando TERMINA SHM ricevuto. Shared memory deallocata\n");

                break;

            case TERMINA_PROCESSO:
                if (shm) {
                    fprintf(stderr, "[Server] Comando inaspettato! Ho ricevuto un comando TERMINA PROCESSO ma non ho mai ricevuto TERMINA SHM in precedenza!\n");
                    exit(1);
                }

                printf("[Server] Comando TERMINA PROCESSO ricevuto\n");
                return;

            default:
                fprintf(stderr, "[Server] Comando non riconosciuto [%ld]\n", comando.type);
                exit(1);
        }
    }
}