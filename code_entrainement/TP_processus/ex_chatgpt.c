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
#include <sys/times.h>
#include <time.h>


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

    if(argc!=2){
        raler(1,"Usage: %s <file> <number of process>",argv[0]);
    }
    int status;
    int number=atoi(argv[1]);
    struct tms tmstart,tmend;
    clock_t start,end;
    pid_t pid;
    CHK(start=times(&tmstart));
    for(int i=0;i<number;i++){
        switch(pid=fork()){
            case -1:
                raler(1,"fork");
            case 0:
                printf("Voila mon PID : %d\n",getpid());
                printf("Voila mon PPID : %d\n",getppid());
                printf("Voila mon UID : %d\n",getuid());
                exit(0);
            default:

        }
    }
    for(int i=0;i<number;i++){
        CHK(pid=wait(&status));
        if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ){
        raler(0, "Le fils a échoué");
        }
    }
    CHK(end=times(&tmend));
    printf("Temps d'execution : %f secondes \n",(double)(end-start)/sysconf(_SC_CLK_TCK));
    return 0;
}