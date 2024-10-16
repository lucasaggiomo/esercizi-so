#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TOKEN_SIZE 20
#define MAX_ARGUMENTS 20
#define BUFFER_SIZE TOKEN_SIZE* MAX_ARGUMENTS

#define MAX_PATH_LENGTH 255
#define MAX_PROGRAM_LENGTH 50

#define MAX_LINES_REMEMBERED 16

void cleanBuffer(FILE* stream);
void readLine(FILE* stream, char* buffer);
int parseArguments(char* buffer, char** args);

int readArguments(FILE* stream, char** args);
int readArgumentsWithLine(FILE* stream, char** args, char* line);

void resolvePath(char* absolutePath, const char* relativePath, const char* cwd);

void increment_value(int* value, const int max);
void decrement_value(int* value, const int max);

int main(int argc, char* argv[]) {
    char cwd[MAX_PATH_LENGTH - MAX_PROGRAM_LENGTH + 1];

    int currLineIndex = 0;
    int headIndex = 0;
    int tailIndex = 0;
    char* prevLines[MAX_LINES_REMEMBERED] = {NULL};

    // printf("Current working dir: %s\n", cwd);
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            exit(EXIT_FAILURE);
        }

        printf("GIOSHELL %s> ", cwd);
        char* arguments[MAX_ARGUMENTS + 1];
        int argcount = readArguments(stdin, arguments);

        char* programPath = arguments[0];
        if (strcmp(programPath, "cd") == 0) {
            // Cambia directory solo se il comando è "cd"
            if (arguments[1] != NULL) {
                // Usa chdir per cambiare la CWD nel processo padre
                if (chdir(arguments[1]) != 0) {
                    perror("chdir failed");
                }
            }
            continue;  // continua
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Errore nell'esecuzione del programma");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // printf("*Figlio che esegue %s*\n", buffer);

            // creo il path come {cwd}/{arguments[0]}
            // char path[MAX_PATH_LENGTH + 1];

            if (programPath[0] != '.' && programPath[0] != '/') {
                // eseguo un programma presente nelle cartelle di sistema (in PATH)
                execvp(arguments[0], arguments);
            } else {
                // eseguo il progrmama presente nel path inserito dall'utente
                char absolutePath[MAX_PATH_LENGTH + 1];

                // ricavo il path assoluto da quello relativo inserito dall'utente
                if (programPath[0] == '.') {
                    resolvePath(absolutePath, programPath, cwd);
                } else
                    strcpy(absolutePath, programPath);

                // printf("Provo ad eseguire:\n%s\n\n", absolutePath);
                // cerco l'eseguibile nel path
                execv(absolutePath, arguments);
            }

            perror("Execution error");
            exit(EXIT_FAILURE);
        }

        wait(NULL);
    }
    return 0;
}

void cleanBuffer(FILE* stream) {
    int ch;
    while ((ch = getc(stream)) != '\n' && ch != EOF)
        ;
}

void readLine(FILE* stream, char* buffer) {
    char format[20];
    snprintf(format, sizeof(format), "%%%d[^\n]", BUFFER_SIZE);
    int status = scanf(format, buffer);

    if (status == EOF || status == 0) {
        cleanBuffer(stream);
        return;
    }
    // printf("You typed \"%s\"\n", buffer);
    cleanBuffer(stream);
}

int parseArguments(char* buffer, char** args) {
    char* token;
    int argc = 0;
    for (token = strtok(buffer, " ");
         token != NULL;
         token = strtok(NULL, " ")) {
        args[argc++] = token;
        // printf("Token:\t%s\n", token);
    }
    return argc;
}

int readArguments(FILE* stream, char** args) {
    char buffer[BUFFER_SIZE + 1];

    readLine(stream, buffer);

    int argc = parseArguments(buffer, args);
    args[argc] = NULL;
    return argc;
}
int readArgumentsWithLine(FILE* stream, char** args, char* line) {
    char buffer[BUFFER_SIZE + 1];

    readLine(stream, buffer);

    int argc = parseArguments(buffer, args);
    args[argc] = NULL;

    strcpy(line, buffer);

    return argc;
}

void resolvePath(char* absolutePath, const char* relativePath, const char* cwd) {
    // Inizializza il percorso assoluto con la CWD
    strncpy(absolutePath, cwd, MAX_PATH_LENGTH);

    char tempPath[MAX_PATH_LENGTH + 1];
    strncpy(tempPath, relativePath, MAX_PATH_LENGTH);

    // Tokenizza il path in segmenti
    for (char* token = strtok(tempPath, "/");
         token != NULL;
         token = strtok(NULL, "/")) {
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
    }
}

void increment_value(int* value, const int max) {
    (*value)++;
    (*value) %= max;
}
void decrement_value(int* value, const int max) {
    (*value)--;
    (*value) %= max;
}