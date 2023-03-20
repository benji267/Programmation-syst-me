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

#define MAX 128

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
    if (argc<3) {
        raler(1, "Usage: %s <file>", argv[0]);
    }
    int status;
    int number = atoi(argv[1]);
    char** args = malloc(sizeof(char*)*(argc));
    if(args==NULL){
        raler(1,"malloc");
    }
    pid_t pid;
    for(int k=2; k<argc;k++){
        args[k-2]=argv[k];
    }
    
    for(int k=0; k<number; k++){
        switch(pid=fork()){
            case -1:
                raler(1,"fork");
            case 0:
                char buf[MAX];
                snprintf(buf, sizeof(MAX), "%d", k );
                args[argc-2]=buf;
                args[argc-1]=NULL;
                execvp(argv[2], args);
                perror("execvp");
            default:     
        }
    }

    for(int k=0; k<number; k++){
        wait(&status);
        if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ){
            raler(0, "Le fils a échoué");
        }
    }
    return 0;

}