#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
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


int main(int argc, char* argv[]){
    if (argc != 2) {
        raler(1, "Usage: %s <file>", argv[0]);
    }
    int raison;
    int number_process=atoi(argv[1]);
    int *tableau_pid=malloc(sizeof(int)*number_process);
    pid_t pid;
    for(int i=0;i<number_process;i++){
        switch(pid=fork()){
            case -1:
                raler(1,"fork");
            case 0:
                tableau_pid[i]=getpid();
                printf("I'm the process number %d and I get the following pid %d\n",i,tableau_pid[i]);
                exit(EXIT_SUCCESS);
        }
    }
    for(int i=0;i<number_process;i++){
        CHK(wait(&raison));
        if (WIFEXITED(raison))
            printf("exit(%d)\n", WEXITSTATUS(raison));
        else if (WIFSIGNALED(raison))
            printf("signal %d\n", WTERMSIG(raison));
        else
            printf("other raison");
    }
}