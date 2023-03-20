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

int random_(int number,int max, int *tab){
    int i;
    for(i=0;i<number;i++){
        tab[i]=1+ rand() % max;
    }
    return 0;
}

int main(int argc, char* argv[]){
    if(argc!=3){
        raler(1,"Usage: %s <file> <number of process>",argv[0]);
    }
    int number=atoi(argv[1]);
    int max=atoi(argv[2]);
    int *tab_number=malloc(sizeof(int)*number);
    random_(number,max,tab_number);
    pid_t pid;
    int sleeptime=0;
    for(int i=0;i<number;i++){
        switch(pid=fork()){
            case -1:
                raler(1,"fork");
            case 0:
                sleeptime=tab_number[i];
                sleep(sleeptime);
                printf("%d\n",sleeptime);
                exit(sleeptime);
            default:
                break;
        }
    }
    int status;
    for(int i=0;i<number;i++){
        CHK(wait(&status));
        if (WIFEXITED(status))
            printf("exit(%d)\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("signal %d\n", WTERMSIG(status));
        else
            printf("other raison");
    }
    free(tab_number);
    return 0;
}