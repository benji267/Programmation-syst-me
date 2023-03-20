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

int main(void){
    int newfd;
    pid_t pid;
    int status;
    switch(pid=fork()){
        case -1:
            raler(1,"fork");
        case 0:
            CHK(newfd=open("toto.txt",O_WRONLY|O_CREAT|O_TRUNC,0666));
            CHK(dup2(newfd,1));
            CHK(close(newfd));
            execlp("ps","ps","eaux",NULL);
            raler(1,"execlp");
        default:
            
    }
    CHK(wait(&status));
    if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ){
        raler(0, "Le fils a échoué");
    }
    int a;
    switch(pid=fork()){
        case -1:
            raler(-1,"fork");
        
        case 0:
            CHK(newfd=open("toto.txt",O_RDONLY));
            CHK(a=open("a.txt",O_WRONLY|O_CREAT|O_TRUNC,0666));
            CHK(dup2(newfd,0));
            CHK(dup2(a,1));
            CHK(close(newfd));
            CHK(close(a));
            execlp("grep","grep", "-v", "root", NULL);
            raler(1,"execlp");
        default:
    }
    CHK(wait(&status));
    if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ){
        raler(0, "Le fils a échoué");
    }
    return 0;
}