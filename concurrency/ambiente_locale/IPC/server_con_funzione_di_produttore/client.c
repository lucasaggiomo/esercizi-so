#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "buffer.h"
#include "messaggi.h"
#include "procedure.h"

int main(int argc, char* argv[]) {
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

    // legge il numero di richieste che deve fare
    if (argc <= 1) {
        fprintf(stderr, "[Client] Errore! Non è stato indicato il numero di richieste\n");
        exit(1);
    }

    int ret;
    int num_richieste = atoi(argv[1]);

    printf("[Client] Inizio a fare %d richieste\n", num_richieste);

    struct msg_comando comando;
    for (int i = 0; i < num_richieste; i++) {
        // invia una richiesta in maniera sincrona al server
        comando.type = RICHIESTA_SHM;
        printf("[Client] Invio un comando di RICHIESTA\n");

        send_sincrona(id_comandi, id_rts_comandi, id_ots_comandi, &comando, SIZEOF_MSG(struct msg_comando));

        // attende una risposta
        struct msg_risposta risposta;
        ret = msgrcv(id_risposte, &risposta, SIZEOF_MSG(struct msg_risposta), 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        printf("[Client] Ho ricevuto una risposta con chiave %d, ne ottengo la memoria condivisa associata\n", risposta.key_shm);
        int id_shm = shmget(risposta.key_shm, sizeof(struct buffer_arr), 0);
        if (id_shm < 0) {
            perror("Errore nella shmget");
            exit(1);
        }

        struct buffer_arr* shm = shmat(id_shm, NULL, 0);
        if (shm == (void*)-1) {
            perror("Errore nella shmat");
            exit(1);
        }

        printf("[Client] Inizio a consumare\n");

        // consuma tutti gli elementi del vettore di buffer
        for (int i = 0; i < risposta.length; i++) {
            struct buffer arr;
            printf("[Client] Sto per consumare l'elemento %d di %d\n", i + 1, risposta.length);

            consuma(shm, &arr);

            printf("[Client] Ho consumato la coppia [%d, %d]\n", arr.val1, arr.val2);
        }

        printf("[Client] Ho terminato i consumi, invio un messaggio di TERMINA SHM al server\n");

        comando.type = TERMINA_SHM;
        send_sincrona(id_comandi, id_rts_comandi, id_ots_comandi, &comando, SIZEOF_MSG(struct msg_comando));
    }

    printf("[Client] Ho effettuato in totale %d RICHIESTE, ora invio un comando di TERMINA PROCESSO al server\n", num_richieste);

    comando.type = TERMINA_PROCESSO;
    send_sincrona(id_comandi, id_rts_comandi, id_ots_comandi, &comando, SIZEOF_MSG(struct msg_comando));

    printf("[Client] Terminazione\n");

    return 0;
}