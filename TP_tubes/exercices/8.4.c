#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

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


int main(int argc, char *argv[]){
    int tube[2];
    pid_t pid;
    char* name;
    if(argc!=2){
        name="USER";
    }
    else{
        name=argv[1];
    }
    CHK(pipe(tube));
    switch(pid=fork()){
        
        case -1:
            raler(1,"fork");
        
        case 0:
            CHK(close(tube[0]));
            CHK(dup2(tube[1],1));
            CHK(close(tube[1]));
            execlp("ps", "ps", "eaux", NULL);
            raler(1,"execlp");
        
        default:
            break;
    }
    size_t taille=strlen(name);
    char arguments[taille+3];
    arguments[0]='^';
    for(size_t i=0;i<taille;i++){
        strcpy(arguments+i+1,name+i);
    }
    arguments[sizeof(name)+1]=' ';
    arguments[sizeof(name)+2]='\0';
    int tube2[2];
    CHK(pipe(tube2));
    switch(pid=fork()){
        case -1:
            raler(1,"fork");
        case 0:
            CHK(dup2(tube2[0],0));
            CHK(dup2(tube2[1],1));
            CHK(close(tube2[0]));
            CHK(close(tube2[1]));
            execlp("grep", "grep", arguments, NULL);    
            raler(1,"execlp");       
        default:
            break;
    }
    
    CHK(close(tube[1]));
    CHK(dup2(tube[0],0));
    CHK(close(tube[0]));
    execlp("wc", "wc", "-l", NULL);
    raler(1,"execlp");
    return 0;
}