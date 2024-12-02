#include <unistd.h>

#include "monitorLS.h"
#include "monitor_hoare.h"

void inizializza(struct MonitorLS* m) {
    init_monitor(&m->monitor, 3);

    m->num_lettori = 0;
    m->num_scrittori_alta_prio = 0;
    m->num_scrittori_bassa_prio = 0;
}

void distruggi(struct MonitorLS* m) {
    remove_monitor(&m->monitor);
}

int leggi(struct MonitorLS* m) {
    enter_monitor(&m->monitor);

    if (m->num_scrittori_alta_prio > 0 || m->num_scrittori_bassa_prio > 0) {
        wait_condition(&m->monitor, CV_LETTORI);
    }

    m->num_lettori++;
    signal_condition(&m->monitor, CV_LETTORI);

    leave_monitor(&m->monitor);

    int lettura = m->buffer;

    enter_monitor(&m->monitor);

    m->num_lettori--;

    // sveglia gli scrittori solo se non ci sono più lettori
    if (m->num_lettori == 0) {
        if (queue_condition(&m->monitor, CV_SCRITTORI_ALTA_PRIO) > 0) {
            signal_condition(&m->monitor, CV_SCRITTORI_ALTA_PRIO);
        } else {
            signal_condition(&m->monitor, CV_SCRITTORI_BASSA_PRIO);
        }
    }

    leave_monitor(&m->monitor);

    return lettura;
}

// bassa priorità
void scrivi_lento(struct MonitorLS* m, int valore) {
    enter_monitor(&m->monitor);

    if (m->num_lettori > 0 || m->num_scrittori_alta_prio > 0 || m->num_scrittori_bassa_prio > 0) {
        wait_condition(&m->monitor, CV_SCRITTORI_BASSA_PRIO);
    }

    m->num_scrittori_bassa_prio++;

    leave_monitor(&m->monitor);

    m->buffer = valore;
    sleep(2);

    enter_monitor(&m->monitor);

    m->num_scrittori_bassa_prio--;

    // segnalo gli scrittori se ce ne sono, dando la priorità a quelli veloci
    // altrimenti segnalo i lettori (nota: l'istruzione di signal condition esegue solo se ci sono lettori in coda)
    if (queue_condition(&m->monitor, CV_SCRITTORI_ALTA_PRIO) > 0) {
        signal_condition(&m->monitor, CV_SCRITTORI_ALTA_PRIO);
    } else if (queue_condition(&m->monitor, CV_SCRITTORI_BASSA_PRIO) > 0) {
        signal_condition(&m->monitor, CV_SCRITTORI_BASSA_PRIO);
    } else {
        signal_condition(&m->monitor, CV_LETTORI);
    }

    leave_monitor(&m->monitor);
}

// alta priorità
void scrivi_veloce(struct MonitorLS* m, int valore) {
    enter_monitor(&m->monitor);

    if (m->num_lettori > 0 || m->num_scrittori_alta_prio > 0 || m->num_scrittori_bassa_prio > 0) {
        wait_condition(&m->monitor, CV_SCRITTORI_ALTA_PRIO);
    }

    m->num_scrittori_bassa_prio++;

    leave_monitor(&m->monitor);

    m->buffer = valore;
    sleep(1);

    enter_monitor(&m->monitor);

    m->num_scrittori_bassa_prio--;

    // segnalo gli scrittori se ce ne sono, dando la priorità a quelli veloci
    // altrimenti segnalo i lettori (nota: l'istruzione di signal condition esegue solo se ci sono lettori in coda)
    if (queue_condition(&m->monitor, CV_SCRITTORI_ALTA_PRIO) > 0) {
        signal_condition(&m->monitor, CV_SCRITTORI_ALTA_PRIO);
    } else if (queue_condition(&m->monitor, CV_SCRITTORI_BASSA_PRIO) > 0) {
        signal_condition(&m->monitor, CV_SCRITTORI_BASSA_PRIO);
    } else {
        signal_condition(&m->monitor, CV_LETTORI);
    }

    leave_monitor(&m->monitor);
}