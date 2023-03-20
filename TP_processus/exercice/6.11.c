#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdnoreturn.h>

#define CHK(op) do { if ((op) == -1) raler (1, #op); } while (0)

noreturn void raler (int syserr, const char *msg, ...)
{
    va_list ap;

    va_start (ap, msg);
    vfprintf (stderr, msg, ap);
    fprintf (stderr, "\n");
    va_end (ap);

    if (syserr == 1)
        perror ("");

    exit (EXIT_FAILURE);
}
#define MAX_LENGTH 1024
#define MAX_ARGS 128

int main(void) {
    while (1) {
    // Attendre une ligne de l'entrée standard
    char line[MAX_LENGTH];
    printf("$ ");
    fgets(line, MAX_LENGTH, stdin);
    // Décomposer la ligne en mots
    char *args[MAX_ARGS];
    char *token = strtok(line, " \n");
    int i = 0;
    while (token != NULL && i < MAX_ARGS) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    // Rechercher le premier mot dans le PATH
    char path[MAX_LENGTH];
    strcpy(path, "/bin/");
    strcat(path, args[0]);
    
   


    // Exécuter la commande
    pid_t pid;
    int status;
    switch(pid=fork()) {
        case -1:
            perror("fork");
            return 1;
        case 0:
            execv(path, args);
            perror("execv");
            return 1;
        default:
        }
        CHK(wait(&status));
        if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ) {
        raler(0, "Erreur exécution ps eaux\n");
    }
    }
    return 0;
}