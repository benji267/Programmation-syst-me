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


//fonction qui affiche le dernier chiffre d'un nombre
int last_digit(int n){
    return n % 10;
}

int main(){

    pid_t pid;
    int raison;

    switch(pid = fork()){
        case -1:
            raler(1, "fork");
        case 0:
            printf("FILS - Je suis le fils, mon PID est %d et celui de mon père est %d\n ", getpid(), getppid());
            exit(last_digit(getpid()));
        default:
            printf("PERE - Mon fils est %d\n", pid);
            CHK(wait(&raison));
            if ( WIFEXITED ( raison )){
                printf ( "PERE - Mon code de retour est %d\n " , WEXITSTATUS ( raison )) ;
            }
            else if ( WIFSIGNALED ( raison )){
                printf ( " signal ␣ % d \n " , WTERMSIG ( raison )) ;
            }
            else{
                printf ( " autre ␣ raison \n " ) ;
            }

    }
}
