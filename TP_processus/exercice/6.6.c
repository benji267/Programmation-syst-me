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
#include <sys/times.h>



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

int main(void){
    char* arg[]={"ls", "-R",NULL};
    pid_t pid;
    int status;
    int devnull;
    struct tms tmsstart, tmsend;
    clock_t start, end;
    CHK(start=times(&tmsstart));
    switch(pid=fork()){
        case -1:
            raler(1, "fork");
        case 0:
            CHK(devnull=open("/dev/null", O_WRONLY));
            CHK(dup2(devnull, 1));
            execvp("ls", arg);
            perror("execvp");
            exit(EXIT_FAILURE);
        default:
    }
    CHK(wait(&status));
    if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ) {
        raler(0, "Erreur exécution ls -R");
    }
    CHK(end=times(&tmsend));
    
    printf("Temps d'éxécutions en secondes %f\n", (double)(end-start)/sysconf(_SC_CLK_TCK));
    return 0;
}
