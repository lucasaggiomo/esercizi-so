#ifndef MONITOR_HOARE_H
#define MONITOR_HOARE_H

struct Monitor {
    // id del semaforo per realizzare il mutex del monitor
    int id_mutex_sem;

    // id del semaforo per realizzare la coda urgent
    int id_urgent_sem;

    // numero di variabili condition
    int num_var_cond;

    // id del vettore di semafori associati alle var.cond
    int id_conds_sem;

    // id della memoria condivisa per i contatori delle variabili condition e della coda urgent
    int id_shared;

    // array delle variabili condition_count
    int* cond_counts;

    // contatore del numero di processi sospesi sulla coda urgent
    int* urgent_count;
};

void init_monitor(struct Monitor* m, int num_var);
void enter_monitor(struct Monitor* m);
void leave_monitor(struct Monitor* m);
void remove_monitor(struct Monitor* m);
void wait_condition(struct Monitor* m, int var_cond_index);
// signal and urgent wait
void signal_condition(struct Monitor* m, int var_cond_index);
// restituisce il numero di processi in coda per la variabile condition
int queue_condition(struct Monitor* m, int var_cond_index);
#endif