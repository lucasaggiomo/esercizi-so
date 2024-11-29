#ifndef BUFFER_H
#define BUFFER_H

struct buffer {
    int ID;
    char str[5];
    int value;
};

#define PRODUTTORE 0
#define CONSUMATORE 1

void produci(int id_sem, struct buffer* dest, const struct buffer* source);
void consuma(int id_sem, struct buffer* dest, const struct buffer* source);

#endif