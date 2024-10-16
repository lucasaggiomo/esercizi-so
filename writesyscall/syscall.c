#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char *strcatArr(char *dest, size_t count, char *arr[])
{
    for (int i = 0; i < count; i++)
    {
        strcat(dest, arr[i]);
        if (i != count - 1)
            strcat(dest, " ");
    }
    strcat(dest, "\n");
    return dest;
}
int main(int argc, char *argv[])
{
    ssize_t result;
    if (argc == 1)
    {
        printf("Inserisci come parametri il contenuto da scrivere sul file");
        return 0;
    }
    // Calcolo la lunghezza totale necessaria per la stringa risultante
    size_t total_length = 1; // +1 per il terminatore '\0'
    for (int i = 1; i < argc; i++)
    {
        total_length += strlen(argv[i]) + 1; // +1 per lo spazio tra le stringhe
    }
    // lo spazio finale lo uso come andata a capo

    // Alloco la memoria necessaria
    char *str = malloc(total_length * sizeof(char));
    if (str == NULL)
    {
        printf("Errore di allocazione della memoria.\n");
        return 1;
    }

    str[0] = '\0';

    str = strcatArr(str, argc - 1, argv + 1);

    printf("Stringa concatenata: {%s}\n", str);

    size_t len = total_length - 1; // tolgo il terminatore

    int fd = open("./file.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);

    asm volatile(
        "mov $4,%%eax;" // $4 = codice di write
        "mov %3,%%ebx;" // %3 = file destinazione
        "mov %1,%%ecx;" // %1 = stringa da scrivere (hello)
        "mov %2,%%edx;" // %2 = lunghezza della stringa (len)
        "int $0x80;"    // syscall
        : "=g"(result)
        : "g"(str), "g"(len), "g"(fd) // %1 = hello, %2 = len, %3 = fd
        : "memory", "eax", "ebx", "ecx", "edx");

    close(fd);

    free(str);
    return 0;
}