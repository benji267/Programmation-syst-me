#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 256

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

void copier(int fdsrc, int fddst){
    char buf[MAX];
    ssize_t n;
    while((n=read(fdsrc,buf,MAX))>0){
        CHK(write(fddst,buf,n));
    }
}

int main(void){
    int tube[2];
    pid_t pid;
    CHK(pipe(tube));
    switch(pid=fork()){
        case -1:
            raler(1,"fork");
        case 0:
            CHK(close(tube[1]));
            copier(tube[0],1);
            CHK(close(tube[0]));
            exit(EXIT_SUCCESS);
        default:  
    }
    CHK(close(tube[0]));
    copier(0,tube[1]);
    CHK(close(tube[1]));
    CHK(wait(NULL));
    return 0;
}