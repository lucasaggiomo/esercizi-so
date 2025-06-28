#ifndef BUFFER_H
#define BUFFER_H

struct buffer {
    int totale;
    int numero_messaggi;

    int id_sem;
};

#define MUTEX 0

void init_buffer(struct buffer* b);
void destroy_buffer(struct buffer* b);
void scrivi(struct buffer* b, int quantita);

#endif