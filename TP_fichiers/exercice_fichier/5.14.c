#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


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

char **separe (char *chaine)
{
    // on compte le nombre de répertoire contenu dans 'chaine'
    int i = 0, rep = 1;
    while (chaine [i] != '\0')
        if (chaine [i++] == ':')
            rep++;

    // création du tableau qui contiendra les noms des répertoires
    char **tab_rep;
    if ((tab_rep = malloc ((rep + 1) * sizeof *tab_rep)) == NULL)
        raler (1, "malloc");
    tab_rep [rep] = NULL;

    i = 0;
    for (int j = 0 ; j < rep ; ) {
        if (chaine [i] == ':' || chaine [i] == '\0') { // on a le nom complet d'un répertoire
            chaine [i] = '\0';

            if ((tab_rep [j] = malloc (strlen (chaine) + 1)) == NULL)
                raler (1, "malloc tab_rep");
            strcpy (tab_rep [j], chaine);

            chaine = &chaine [i] + 1;                  // on passe au répertoire suivant
            i = 0;
            j++;
        }
        else
            i++;
    }

    return tab_rep;
}


int which(const char *prog)
{
    char *path = getenv("PATH");
    if (path == NULL){
        raler (0, "PATH non défini");
    }
    char **tab_rep = separe (path);
    char *chemin;
    for (int i = 0 ; tab_rep [i] != NULL ; i++) {
        if ((chemin = malloc (strlen (tab_rep [i]) + strlen (prog) + 2)) == NULL)
            raler (1, "malloc chemin");
        strcpy (chemin, tab_rep [i]);
        strcat (chemin, "/");
        strcat (chemin, prog);
        if (access (chemin, X_OK) == 0) {
            printf ("%s\n", chemin);
            return 0;
        }
    }
    free(chemin);

    return 1;
}

int main (int argc, char *argv [])
{
    if (argc != 2)
        raler (0, "usage: %s prog", argv [0]);

    if (which (argv [1]) == 1)
        raler (0, "%s: commande introuvable", argv [1]);

    return EXIT_SUCCESS;
}
