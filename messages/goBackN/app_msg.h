#include <sys/types.h>

// struttura per i messaggi tra i livelli applicativi
struct app_msg {
    long type;
    char character;
};

#define SIZE_APP_MSG (sizeof(struct app_msg) - sizeof(long))
