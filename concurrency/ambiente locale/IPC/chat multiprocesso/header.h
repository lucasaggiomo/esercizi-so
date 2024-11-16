#ifndef __HEADER__
#define __HEADER__

// Path per la FTOK
#define FTOK_PATH "."

// Tipo per il messaggio
#define TYPE 1

#define EXIT_STRING "exit"

// Struct relativa ai messaggi
struct msg {
    long msgType;
    char message[20];
};

#define SIZE_MSG (sizeof(struct msg) - sizeof(long))

void sender(int id_queue_receiver, int id_queue_sender);
void receiver(int id_queue_receiver);

#endif // __HEADER__