#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


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

int main(int argc, char *argv[]) {
    int i, n, status, ret = 0;
    pid_t pid;

    // Vérifiez si les arguments sont valides
    if (argc < 3) {
        raler(0, "Usage: %s n cmd [arg...]", argv[0]);
    }

    // Récupérez la valeur de n
    n = atoi(argv[1]);

    // Bouclez à travers les n commandes
    for (i = 0; i < n; i++){
        switch(pid = fork()){

            case -1:
                raler(1, "fork");


            // Le processus fils exécute la commande
            case 0:
                // Allouez de la mémoire pour les arguments supplémentaires
                char **args = malloc((argc) * sizeof(char *));
                if (args == NULL) {
                    perror("malloc");
                    return 1;
                }

                // Copiez les arguments
                int j;
                for (j = 2; j < argc; j++) {
                    args[j - 2] = argv[j];
                }

                // Ajoutez le rang d'appel i en tant qu'argument supplémentaire
                char buf[16];
                snprintf(buf, sizeof(buf), "%d", i);
                args[argc - 2] = buf;
                args[argc - 1] = NULL;

                // Exécutez la commande
                execvp(argv[2], args);

                // Si execvp échoue, affichez un message d'erreur
                perror("execvp");
                break;
            
            default:
                break;
        }
    }

    // Le processus parent attend les n processus fils
    for (i = 0; i < n; i++) {
        CHK(wait(&status));


        // Mettez à jour le code de retour si une commande échoue
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            ret = 1;
        }
    }

    return ret;
}
