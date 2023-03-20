#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
    int tube[2];
    pid_t pid;
    int status;
    char buf[100];
    CHK(pipe(tube));
    switch(pid=fork()){
        case -1:
            raler(1,"fork");
        case 0:
            CHK(close(tube[0]));
            CHK(write(tube[1],"-FILS bonjour père\n",19));
            CHK(close(tube[1]));
            exit(EXIT_SUCCESS);
        default:
            CHK(close(tube[1]));
            CHK(read(tube[0],buf,100));
            CHK(close(tube[0]));
            printf("%s %d %s\n",buf,pid,"-PERE bonjour fils\n");
    }
    CHK(wait(&status));
    return 0;
}