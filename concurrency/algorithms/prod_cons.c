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

#pragma region BUFFER SINGOLO
// Semafori
struct buffer_os {
    void* data;
};

#define SPAZIO_DISP 0
#define MESSAGGIO_DISP 1

void produci_os(int id_sem, struct buffer_os* b, void* data, size_t size) {
    Wait_Sem(id_sem, SPAZIO_DISP);

    memcpy(b, data, size);

    Signal_Sem(id_sem, MESSAGGIO_DISP);
}

void consuma_os(int id_sem, struct buffer_os* b, void* data, size_t size) {
    Wait_Sem(id_sem, MESSAGGIO_DISP);

    memcpy(data, b, size);

    Signal_Sem(id_sem, SPAZIO_DISP);
}

// Monitor signal and wait/continue
struct buffer_omw {
    void* data;
    int libero;

    Monitor m;
};
#define CV_PRODUTTORE 0
#define CV_CONSUMATORE 1

void produci_omw(int id_sem, struct buffer_omw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (!b->libero) {     // while
        wait_cond(&b->m, CV_PRODUTTORE);
    }

    memcpy(b->data, data, size);
    b->libero = 0;

    signal_cond(&b->m, CV_CONSUMATORE);

    leave_monitor(&b->m);
}

void consuma_omw(int id_sem, struct buffer_omw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->libero) {     // while
        wait_cond(&b->m, CV_CONSUMATORE);
    }

    memcpy(data, b->data, size);
    b->libero = 1;

    signal_cond(&b->m, CV_PRODUTTORE);

    leave_monitor(&b->m);
}

#pragma endregion

#pragma region CODA CIRCOLARE - PROD CONS MULTIPLI(se singoli togli il MUTEX rispettivo)
// Semafori
#define DIM 10
struct buffer_cs {
    void* data[DIM];
    int testa;
    int coda;
};

#define SPAZIO_DISP 0
#define MESSAGGIO_DISP 1
#define MUTEX_P 2
#define MUTEX_C 3

void produci_c(int id_sem, struct buffer_cs* b, void* data, size_t size) {
    Wait_Sem(id_sem, SPAZIO_DISP);
    Wait_Sem(id_sem, MUTEX_P);

    memcpy(b->data[b->testa], data, size);
    b->testa = (b->testa + 1) % DIM;

    Signal_Sem(id_sem, MUTEX_P);
    Signal_Sem(id_sem, MESSAGGIO_DISP);
}

void consuma_c(int id_sem, struct buffer_cs* b, void* data, size_t size) {
    Wait_Sem(id_sem, MESSAGGIO_DISP);
    Wait_Sem(id_sem, MUTEX_C);

    memcpy(data, b->data[b->coda], size);
    b->coda = (b->coda + 1) % DIM;

    Signal_Sem(id_sem, MUTEX_C);
    Signal_Sem(id_sem, SPAZIO_DISP);
}

// Monitor signal and wait/continue
struct buffer_cmw {
    void* data[DIM];
    int testa;
    int coda;
    int contatore;

    Monitor m;
};
#define CV_PRODUTTORE 0
#define CV_CONSUMATORE 1

void produci_cmw(int id_sem, struct buffer_cmw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->contatore == DIM) {     // while
        wait_cond(&b->m, CV_PRODUTTORE);
    }

    memcpy(b->data[b->testa], data, size);
    b->testa = (b->testa + 1) % DIM;
    b->contatore++;

    signal_cond(&b->m, CV_CONSUMATORE);

    leave_monitor(&b->m);
}

void consuma_cmw(int id_sem, struct buffer_cmw* b, void* data, size_t size) {
    enter_monitor(&b->m);

    if (b->contatore == 0) {     // while
        wait_cond(&b->m, CV_CONSUMATORE);
    }

    memcpy(data, b->data[b->coda], size);
    b->coda = (b->coda + 1) % DIM;
    b->contatore--;

    signal_cond(&b->m, CV_PRODUTTORE);

    leave_monitor(&b->m);
}
#pragma endregion

#pragma region VETTORE DI STATO
// Semafori
#define DIM 10
enum stato {
    LIBERO,
    OCCUPATO,
    IN_USO
};
struct buffer_ss {
    void* data[DIM];
    enum stato stato[DIM];
};

#define SPAZIO_DISP 0
#define MESSAGGIO_DISP 1
#define MUTEX_P 2
#define MUTEX_C 3

void produci_s(int id_sem, struct buffer_ss* b, void* data, size_t size) {
    int index = 0;

    Wait_Sem(id_sem, SPAZIO_DISP);
    Wait_Sem(id_sem, MUTEX_P);

    while (b->stato[index] != LIBERO && index < DIM) {
        index++;
    }
    b->stato[index] = IN_USO;

    Signal_Sem(id_sem, MUTEX_P);

    memcpy(b->data[index], data, size);
    b->stato[index] = OCCUPATO;

    Signal_Sem(id_sem, MESSAGGIO_DISP);
}

void consuma_s(int id_sem, struct buffer_ss* b, void* data, size_t size) {
    int index = 0;

    Wait_Sem(id_sem, MESSAGGIO_DISP);
    Wait_Sem(id_sem, MUTEX_C);

    while (b->stato[index] != OCCUPATO && index < DIM) {
        index++;
    }
    b->stato[index] = IN_USO;

    Signal_Sem(id_sem, MUTEX_C);

    memcpy(data, b->data[index], size);
    b->stato[index] = LIBERO;

    Signal_Sem(id_sem, SPAZIO_DISP);
}

// Monitor signal and wait/continue
struct buffer_smw {
    void* data[DIM];
    enum stato stato[DIM];
    int num_liberi;
    int num_occupati;

    Monitor m;
};
#define CV_PRODUTTORE 0
#define CV_CONSUMATORE 1

void produci_smw(int id_sem, struct buffer_smw* b, void* data, size_t size) {
    int index = 0;

    enter_monitor(&b->m);

    if (b->num_liberi == 0) {     // while
        wait_cond(&b->m, CV_PRODUTTORE);
    }

    while (b->stato[index] != LIBERO && index < DIM) {
        index++;
    }
    b->stato[index] = IN_USO;
    b->num_liberi--;

    leave_monitor(&b->m);

    memcpy(b->data[index], data, size);

    enter_monitor(&b->m);

    b->stato[index] = OCCUPATO;
    b->num_occupati++;

    signal_cond(&b->m, CV_CONSUMATORE);

    leave_monitor(&b->m);
}

void consuma_smw(int id_sem, struct buffer_smw* b, void* data, size_t size) {
    int index = 0;

    enter_monitor(&b->m);

    if (b->num_occupati == 0) {     // while
        wait_cond(&b->m, CV_CONSUMATORE);
    }

    while (b->stato[index] != OCCUPATO && index < DIM) {
        index++;
    }
    b->stato[index] = IN_USO;
    b->num_occupati--;

    leave_monitor(&b->m);

    memcpy(data, b->data[index], size);

    enter_monitor(&b->m);

    b->stato[index] = LIBERO;
    b->num_liberi++;

    signal_cond(&b->m, CV_PRODUTTORE);

    leave_monitor(&b->m);
}
