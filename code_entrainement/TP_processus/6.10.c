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


char **separe(char *chaine, char delimiter)
{
    // on compte le nombre de répertoire contenu dans 'chaine'
    int i = 0, rep = 1;
    while (chaine[i] != '\0')
        if (chaine[i++] == delimiter)
            rep++;

    // création du tableau qui contiendra les noms des répertoires
    char **tab_rep;
    if ((tab_rep = malloc((rep + 1) * sizeof *tab_rep)) == NULL)
        raler(1, "malloc");
    tab_rep[rep] = NULL;

    i = 0;
    for (int j = 0; j < rep;)
    {
        if (chaine[i] == delimiter || chaine[i] == '\0')
        { // on a le nom complet d'un répertoire
            chaine[i] = '\0';

            if ((tab_rep[j] = malloc(strlen(chaine) + 1)) == NULL)
                raler(1, "malloc tab_rep");
            strcpy(tab_rep[j], chaine);

            chaine = &chaine[i] + 1; // on passe au répertoire suivant
            i = 0;
            j++;
        }
        else
            i++;
    }

    return tab_rep;
}



int main(void) {
    while (1) {
    // Attendre une ligne de l'entrée standard
    char line[MAX_LENGTH];
    printf("$ ");
    fgets(line, MAX_LENGTH, stdin);
    // Décomposer la ligne en mots
    char **args;
    args=malloc(sizeof(char*)*MAX_ARGS);
    if(args==NULL){
        raler(1,"malloc");
    }
    args=separe(line, ' ');
    int i = 0;
    while(args[i] != NULL){
        i++;
    }
    // placé NULL à la fin du tableau
    args[i-1] = NULL;

    // Rechercher le premier mot dans le PATH
    //char path[MAX_LENGTH];
    //snprintf(path, MAX_LENGTH, "/bin/%s", args[0]);

    char* ge_path=getenv("PATH");
    if(ge_path==NULL){
        raler(1,"getenv");
    }
    char* path;
    path=malloc((strlen (ge_path) + 1));
    if(path==NULL){
        raler(1,"malloc");
    }

    if( access(path, X_OK) == -1 ) {
        raler(0, "Commande inconnue\n");
    }
    // Exécuter la commande
    pid_t pid;
    int status;
    switch(pid=fork()) {
        case -1:
            perror("fork");
            return 1;
        case 0:
            execv(ge_path, args);
            perror("execv");
            return 1;
        default:
        }
        CHK(wait(&status));
        if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ) {
        raler(0, "Erreur exécution\n");
        }
    }
    return 0;
}