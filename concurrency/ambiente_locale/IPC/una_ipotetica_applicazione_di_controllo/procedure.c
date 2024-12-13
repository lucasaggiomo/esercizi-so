#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "messaggi.h"

#include "procedure.h"

static int id_log_interfaccia;

size_t safe_strcpy(char* dest, const char* source, size_t dest_dim) {
    size_t i = 0;

    if (dest_dim == 0)
        return 0;

    while (i < dest_dim - 1) {
        dest[i] = source[i];
        if (source[i] == '\0') {
            return i;
        }
        i++;
    }

    dest[i] = '\0';
    return i;
}

void log_message(const char* message) {
    struct msg_log log = {
        .type = 1
    };
    safe_strcpy(log.message, message, DIM_LOG_MESSAGE);

    int ret = msgsnd(id_log_interfaccia, &log, SIZE_MSG_LOG, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }
}

void termina_macchina(const char* message) {
    struct msg_log log = {
        .type = TERMINAZIONE
    };
    safe_strcpy(log.message, message, DIM_LOG_MESSAGE);

    int ret = msgsnd(id_log_interfaccia, &log, SIZE_MSG_LOG, 0);
    if (ret < 0) {
        perror("Errore nella msgsnd");
        exit(1);
    }
}

void controllore(int id_comandi_macchina,
                 int id_log_interfaccia_parametro,
                 int id_rts_termostato,
                 int id_ots_termostato,
                 int id_comandi_termostato,
                 int id_feedback_termostato,
                 int id_rts_mixer,
                 int id_ots_mixer,
                 int id_comandi_mixer) {

    id_log_interfaccia = id_log_interfaccia_parametro;
    // attende la ricezione di un comando di avvio dall'interfaccia
    int ret;
    struct msg_comando comando;
    ret = msgrcv(id_comandi_macchina, &comando, SIZE_MSG_COMANDO, 0, 0);
    if (ret < 0) {
        perror("Errore nella msgrcv");
        exit(1);
    }

    if (comando.type != AVVIO_PREPARAZIONE) {
        fprintf(stderr, "Ho ricevuto un comando non riconosciuto (%ld)\n", comando.type);
        exit(1);
    }

    printf("[Controllore] Ho ricevuto il comando di AVVIO PREPARAZIONE, quindi invio un comando di AVVIO ROTAZIONE al mixer...\n");

    // invia un comando di AVVIO ROTAZIONE al processo Mixer con una send sincrona
    comando.type = AVVIO_ROTAZIONE;
    send_sincrona(id_rts_mixer, id_ots_mixer, id_comandi_mixer, &comando, SIZE_MSG_COMANDO);

    log_message("Avvio rotazione\n");

    // attende 3 secondi e poi invia un comando di INTERRUZIONE ROTAZIONE al Mixer con una send sincrona
    sleep(3);

    printf("[Controllore] Sono passati 3 secondi, invio un comando di INTERRUZIONE ROTAZIONE al mixer...\n");

    comando.type = INTERRUZIONE_ROTAZIONE;
    send_sincrona(id_rts_mixer, id_ots_mixer, id_comandi_mixer, &comando, SIZE_MSG_COMANDO);

    log_message("Interruzione rotazione\n");

    // invia un comando di AVVIO RISCALDAMENTO al processo Termostato con una send sincrona
    comando.type = AVVIO_RISCALDAMENTO;
    send_sincrona(id_rts_termostato, id_ots_termostato, id_comandi_termostato, &comando, SIZE_MSG_COMANDO);

    log_message("Avvio riscaldamento\n");

    // si mette in ascolto sul valore della temperatura inviatagli dal processo Termostato
    struct msg_feedback temp;
    temp.data = 15;
    while (temp.data < 50) {
        ret = msgrcv(id_feedback_termostato, &temp, SIZE_MSG_FEEDBACK, 0, 0);
        if (ret < 0) {
            perror("Errore nella msgrcv");
            exit(1);
        }

        char buffer[DIM_LOG_MESSAGE];
        snprintf(buffer, DIM_LOG_MESSAGE, "Temperatura: %d °C\n", temp.data);
        log_message(buffer);
    }

    // invia un comando di INTERRUZIONE RISCALDAMENTO al processo Termostato con una send sincrona
    comando.type = INTERRUZIONE_RISCALDAMENTO;
    send_sincrona(id_rts_termostato, id_ots_termostato, id_comandi_termostato, &comando, SIZE_MSG_COMANDO);

    log_message("Interruzione riscaldamento\n");

    // invia messaggi di TERMINAZIONE a termostato, mixer e interfaccia
    comando.type = TERMINAZIONE;

    printf("[Controllore] Invio messaggio di TERMINAZIONE al mixer\n");
    send_sincrona(id_rts_mixer, id_ots_mixer, id_comandi_mixer, &comando, SIZE_MSG_COMANDO);
    printf("[Controllore] Invio messaggio di TERMINAZIONE al termostato\n");
    send_sincrona(id_rts_termostato, id_ots_termostato, id_comandi_termostato, &comando, SIZE_MSG_COMANDO);

    printf("[Controllore] Invio messaggio di TERMINAZIONE all'interfaccia\n");
    termina_macchina("La preparazione è terminata\n");

    printf("[Controllore] Terminazione\n");
}

void termostato(int id_rts,
                int id_ots,
                int id_comandi,
                int id_feedback) {
    int ret;

    int sta_riscaldando = 0;
    int temperatura = 15;     // temperatura iniziale

    while (1) {
        struct msg_comando comando;

        // attende un comando dal controller
        ret = receive_sincrona(id_rts, id_ots, id_comandi, &comando, SIZE_MSG_COMANDO, IPC_NOWAIT);

        if (ret == 0) {
            switch (comando.type) {
                case AVVIO_RISCALDAMENTO:
                    if (sta_riscaldando) {
                        printf("[Termostato] Comando imprevisto! Ho ricevuto un comando di AVVIO RISCALDAMENTO, ma sto già riscaldando\n");
                        exit(1);
                    }
                    sta_riscaldando = 1;
                    break;
                case INTERRUZIONE_RISCALDAMENTO:
                    if (!sta_riscaldando) {
                        printf("[Termostato] Comando imprevisto! Ho ricevuto un comando di INTERRUZIONE RISCALDAMENTO, ma non sto riscaldando\n");
                        exit(1);
                    }
                    sta_riscaldando = 0;
                    break;
                case TERMINAZIONE:
                    if (sta_riscaldando) {
                        printf("[Termostato] Comando imprevisto! Ho ricevuto un comando di TERMINAZIONE, ma sto ancora riscaldando\n");
                        exit(1);
                    }
                    return;
            }
        }

        if (sta_riscaldando) {
            temperatura += 3;

            printf("[Termostato] La temperatura è aumentata di 3 °C, informo il controllore della nuova temperatura\n");
            struct msg_feedback feedback = {
                .type = 1,
                .data = temperatura
            };
            ret = msgsnd(id_feedback, &feedback, SIZE_MSG_FEEDBACK, 0);
            if (ret < 0) {
                perror("Errore nella msgsnd");
                exit(1);
            }

            sleep(1);
        }
    }
}

void mixer(int id_rts,
           int id_ots,
           int id_comandi) {

    int sta_ruotando = 0;

    while (1) {
        struct msg_comando comando;

        // attende un comando dal controller
        receive_sincrona(id_rts, id_ots, id_comandi, &comando, SIZE_MSG_COMANDO, 0);

        switch (comando.type) {
            case AVVIO_ROTAZIONE:
                if (sta_ruotando) {
                    printf("[Mixer] Comando imprevisto! Ho ricevuto un comando di AVVIO ROTAZIONE, ma sto già ruotando\n");
                    exit(1);
                }
                sta_ruotando = 1;
                break;
            case INTERRUZIONE_ROTAZIONE:
                if (!sta_ruotando) {
                    printf("[Mixer] Comando imprevisto! Ho ricevuto un comando di INTERRUZIONE ROTAZIONE, ma non sto ruotando\n");
                    exit(1);
                }
                sta_ruotando = 0;
                break;
            case TERMINAZIONE:
                if (sta_ruotando) {
                    printf("[Mixer] Comando imprevisto! Ho ricevuto un comando di TERMINAZIONE, ma sto ancora ruotando\n");
                    exit(1);
                }
                return;
        }
    }
}