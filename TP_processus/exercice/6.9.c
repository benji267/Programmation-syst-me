#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <unistd.h> // fork et autre
#include <sys/wait.h> // wait
#include <sys/stat.h> // open
#include <fcntl.h> // open ?
#include <stdint.h> // intmax_t et uintmax_t
#include <signal.h>

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
    if(argc != 2) {
        raler(0,"Mauvais arguments. Utilisation : %s <>\n", argv[0]);
    }

    int toto;
    switch(fork()) {
        case -1:
            raler(1,"fork");
        case 0: //fils
            CHK(toto = open("toto", O_WRONLY | O_CREAT | O_TRUNC, 0666));
            CHK(dup2(toto, 1)); // remplacement de stdout par toto
            CHK(close(toto));
            execlp("ps", "ps", "eaux", NULL);
            raler(1, "execlp ps");
        default: // père
            break;
    }

    int raison;
    CHK(wait(&raison));
    int size=0;
    while(argv[1][size] != '\0') {
        size++;
    }
    char argument[size+3];

    int devnull;
    switch(fork()) {
        case -1:
            raler(1, "fork 2");
        case 0: //fils
            
            CHK(toto = open("toto", O_RDONLY));
            CHK(devnull = open("/dev/null", O_WRONLY));
            CHK(dup2(toto, 0)); // remplacement de stdin par toto
            CHK(dup2(devnull, 1)); // remplacement de stdout par /dev/null
            CHK(close(toto));
            CHK(close(devnull));
            argument[0] = '^';
            for(int i=0;i<size;i++) {
                argument[i+1] = argv[1][i];
            }
            argument[size+1] = ' ';
            argument[size+2] = '\0';
            execlp("grep", "grep", argument, NULL);
            raler(1, "execlp grep");
            break;
        default:
            break;
    }
    CHK(wait(&raison));
    if(!WIFEXITED(raison)) {
        raler(0,"Erreur exécution grep");
    }
    if(WEXITSTATUS(raison) == EXIT_SUCCESS) {
        CHK(write(1, argv[1],size ));
        CHK(write(1, " est connecté\n", 15));
    }
    else {
        exit(EXIT_FAILURE);
    }
    return 0;
}