#include <string.h>

extern void Wait_Sem(int id_sem, int num_sem);
extern void Signal_Sem(int id_sem, int num_sem);

typedef unsigned long int size_t;

typedef struct {
} Monitor;

extern void enter_monitor(Monitor* m);
extern void wait_cond(Monitor* m, int var_cond);
extern void signal_cond(Monitor* m, int var_cond);
extern int queue_cond(Monitor* m, int var_cond);
extern void signal_all_cond(Monitor* m, int var_cond);
extern void leave_monitor(Monitor* m);

#pragma region STARVATION SOLI SCRITTORI
// Semafori
struct buffer_s {
    void* data;
    int num_lettori;
};

#define MUTEX_L 0
#define SYNCH 1

void leggi_s(int id_sem, struct buffer_s* b, void* data, size_t size) {
    Wait_Sem(id_sem, MUTEX_L);
    b->num_lettori++;
    if (b->num_lettori == 1) {
        Wait_Sem(id_sem, SYNCH);
    }
    Signal_Sem(id_sem, MUTEX_L);

    memcpy(data, b->data, size);

    Wait_Sem(id_sem, MUTEX_L);
    b->num_lettori--;
    if (b->num_lettori == 0) {
        Signal_Sem(id_sem, SYNCH);
    }
    Signal_Sem(id_sem, MUTEX_L);
}

void scrivi_s(int id_sem, struct buffer_s* b, void* data, size_t size) {
    Wait_Sem(id_sem, SYNCH);

    memcpy(b->data, data, size);

    Signal_Sem(id_sem, MUTEX_L);
}

// Monitor signal and wait/continue
struct buffer_smw {
    void* data;
    int num_lettori;

    Monitor m;
};

#define CV_SCRITTORI 0

void leggi_smw(int id_sem, struct buffer_smw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    b->num_lettori++;

    leave_monitor(&b->m);

    memcpy(data, b->data, size);

    enter_monitor(&b->m);

    b->num_lettori--;

    leave_monitor(&b->m);
}

void scrivi_smw(int id_sem, struct buffer_smw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->num_lettori > 0) {
        wait_cond(&b->m, CV_SCRITTORI);
    }

    memcpy(b->data, data, size);

    leave_monitor(&b->m);
}
#pragma endregion

#pragma region STARVATION SCRITTORI E LETTORI
// Semafori

struct buffer_ls {
    void* data;
    int num_lettori;
    int num_scrittori;
};

#define MUTEX_L 0
#define MUTEX_S 1
#define MUTEX_RISORSA 2
#define SYNCH 1

void leggi_ls(int id_sem, struct buffer_ls* b, void* data, size_t size) {
    Wait_Sem(id_sem, MUTEX_L);
    b->num_lettori++;
    if (b->num_lettori == 1) {
        Wait_Sem(id_sem, SYNCH);
    }
    Signal_Sem(id_sem, MUTEX_L);

    memcpy(data, b->data, size);

    Wait_Sem(id_sem, MUTEX_L);
    b->num_lettori--;
    if (b->num_lettori == 0) {
        Signal_Sem(id_sem, SYNCH);
    }
    Signal_Sem(id_sem, MUTEX_L);
}

void scrivi_ls(int id_sem, struct buffer_ls* b, void* data, size_t size) {
    Wait_Sem(id_sem, MUTEX_S);
    b->num_scrittori++;
    if (b->num_scrittori == 1) {
        Wait_Sem(id_sem, SYNCH);
    }
    Signal_Sem(id_sem, MUTEX_S);

    Wait_Sem(id_sem, MUTEX_RISORSA);

    memcpy(data, b->data, size);

    Signal_Sem(id_sem, MUTEX_RISORSA);

    Wait_Sem(id_sem, MUTEX_S);
    b->num_scrittori--;
    if (b->num_scrittori == 0) {
        Signal_Sem(id_sem, SYNCH);
    }
    Signal_Sem(id_sem, MUTEX_S);
}

// Monitor signal and wait
struct buffer_lsmw {
    void* data;
    int num_lettori;
    int num_scrittori;

    Monitor m;
};

#define CV_LETTORI 0
#define CV_SCRITTORI 1

void leggi_lsmw(int id_sem, struct buffer_lsmw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->num_scrittori > 0) {
        wait_cond(&b->m, CV_LETTORI);
    }
    b->num_lettori++;

    signal_cond(&b->m, CV_LETTORI);

    leave_monitor(&b->m);

    memcpy(data, b->data, size);

    enter_monitor(&b->m);

    b->num_lettori--;
    if (b->num_lettori == 0) {
        signal_cond(&b->m, CV_SCRITTORI);
    }

    leave_monitor(&b->m);
}

void scrivi_lsmw(int id_sem, struct buffer_lsmw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->num_scrittori > 0 || b->num_lettori > 0) {
        wait_cond(&b->m, CV_SCRITTORI);
    }
    b->num_scrittori++;

    leave_monitor(&b->m);

    memcpy(b->data, data, size);

    enter_monitor(&b->m);

    b->num_scrittori--;
    if (queue_cond(&b->m, CV_SCRITTORI)) {
        signal_cond(&b->m, CV_SCRITTORI);
    } else if (queue_cond(&b->m, CV_LETTORI)) {     // condizione superflua
        signal_cond(&b->m, CV_LETTORI);
    }

    leave_monitor(&b->m);
}

// Monitor signal and continue (es: pthread, ma in tal caso vanno usati
// contatori per cv, non esiste queue_cond) Nota: il seguente algoritmo è
// un'alternativa all'algoritmo precedente nel caso signal and continue quindi
// teoricamente si potrebbe usare comunuque l'algoritmo in precedenza anche in
// caso di semantica signal and continue
struct buffer_lsmc {
    void* data;
    int num_lettori;
    int num_scrittori;

    Monitor m;
};

#define CV_LETTORI 0
#define CV_SCRITTORI 1

void leggi_lsmc(int id_sem, struct buffer_lsmc* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->num_scrittori > 0) {
        wait_cond(&b->m, CV_LETTORI);
    }
    b->num_lettori++;

    leave_monitor(&b->m);

    memcpy(data, b->data, size);

    enter_monitor(&b->m);

    b->num_lettori--;
    if (b->num_lettori == 0) {
        signal_cond(&b->m, CV_SCRITTORI);
    }

    leave_monitor(&b->m);
}

void scrivi_lsmc(int id_sem, struct buffer_lsmc* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->num_scrittori > 0 || b->num_lettori > 0) {
        wait_cond(&b->m, CV_SCRITTORI);
    }
    b->num_scrittori++;

    leave_monitor(&b->m);

    memcpy(b->data, data, size);

    enter_monitor(&b->m);

    b->num_scrittori--;
    if (queue_cond(&b->m, CV_SCRITTORI)) {
        signal_cond(&b->m, CV_SCRITTORI);
    } else if (queue_cond(&b->m, CV_LETTORI)) {     // condizione superflua
        signal_all_cond(&b->m, CV_LETTORI);
    }

    leave_monitor(&b->m);
}

#pragma endregion
