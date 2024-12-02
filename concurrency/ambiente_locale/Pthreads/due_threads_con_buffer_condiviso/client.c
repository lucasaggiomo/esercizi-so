#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "messaggi.h"
#include "wrapper.h"

#define NUM_CICLI_LETTURA 3
#define NUM_LETTURE 5

int main() {
    key_t key_comandi = ftok(".", 'r');
    int id_comandi = Msgget(key_comandi, 0);

    struct msg_comando com;

    for (int k = 0; k < NUM_CICLI_LETTURA; k++) {
        // alloca una nuova coda di ricezione
        key_t key_data = ftok(".", 'a' + k);     // 'a', 'b' e 'c'
        int id_data = Msgget(key_data, IPC_CREAT | 0644);

        printf("[Client] Mando comando INIZIO LETTURA, chiave = %d\n", key_data);

        // invia comando di INIZIO LETTURA
        struct msg_comando com;
        com.type = INIZIO_LETTURA;
        com.key_coda = key_data;
        Msgsnd(id_comandi, &com, SIZE_MSG_COMANDO, 0);

        // effettua 5 letture
        for (int i = 0; i < NUM_LETTURE; i++) {
            struct msg_data data;
            Msgrcv(id_data, &data, SIZE_MSG_DATA, 0, 0);

            printf("[Client] Ho letto %d\n", data.value);
        }

        printf("[Client] Mando comando FINE LETTURA\n");

        // invia comando di FINE LETTURA
        com.type = FINE_LETTURA;
        Msgsnd(id_comandi, &com, SIZE_MSG_COMANDO, 0);

        // dealloca la coda
        Msgctl(id_data, IPC_RMID, NULL);
    }

    printf("[Client] Mando comando TERMINA SESSIONE\n");

    // invia un comando di terminazione al server
    com.type = TERMINA_SESSIONE;
    Msgsnd(id_comandi, &com, SIZE_MSG_COMANDO, 0);
}
