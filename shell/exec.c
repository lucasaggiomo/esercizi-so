#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 255

void resolvePath(char* absolutePath, const char* relativePath, const char* cwd);

int main() {
    char cwd[MAX_PATH_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }
    printf("cwd: %s\n", cwd);
    char relativePath[MAX_PATH_LENGTH] = "././././test/././dir";
    char absolutePath[MAX_PATH_LENGTH];

    resolvePath(absolutePath, relativePath, cwd);
    printf("Absolute Path: %s\n", absolutePath);

    return 0;
}

void resolvePath(char* absolutePath, const char* relativePath, const char* cwd) {
    // Inizializza il percorso assoluto con la CWD
    strncpy(absolutePath, cwd, MAX_PATH_LENGTH);

    char tempPath[MAX_PATH_LENGTH + 1];
    strncpy(tempPath, relativePath, MAX_PATH_LENGTH);

    // Tokenizza il path in segmenti
    char* token = strtok(tempPath, "/");
    while (token != NULL) {
        if (strcmp(token, "..") == 0) {
            // ".." = vai alla cartella padre, rimuovendo l'ultimo segmento dal percorso
            char* lastSlash = strrchr(absolutePath, '/');  // puntatore all'ultima occorrenza di '/' in absolutePath
            if (lastSlash != NULL) {
                *lastSlash = '\0';  // Tronca al precedente "/"
            }
        } else if (strcmp(token, ".") != 0) {
            // "." = rimani nella stessa directory
            // Se è un altro segmento (nome di directory o file), aggiungilo al percorso
            strncat(absolutePath, "/", MAX_PATH_LENGTH - strlen(absolutePath) - 1);
            strncat(absolutePath, token, MAX_PATH_LENGTH - strlen(absolutePath) - 1);
        }
        token = strtok(NULL, "/");
    }
}
