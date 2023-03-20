#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define CHK(op)               \
    do {                      \
        if ((op) == -1)       \
            raler(1, #op); \
    } while (0)

noreturn void raler(int syserr, const char *msg, ...) {
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1)
        perror("");

    exit(EXIT_FAILURE);
}


void copy(int fdsrc, int fddst){
    char buf[BUFSIZ];
    ssize_t n;
    while ((n = read(fdsrc, buf, BUFSIZ)) > 0) {
        CHK(write(fddst, buf, n));
    }
    if(n == -1)
        raler(1, "read");
}

int main(int argc, char *argv[]){
    if(argc!=2){
        raler(1, "Usage: %s <file>", argv[0]);
    }
    int summ=0;
    int number_process=atoi(argv[1]);
    int status;
    pid_t pid;
    //create a tab of pipe 
    int pipefd[number_process][2];
    int i;
    for(i=0;i<number_process;i++){
        CHK(pipe(pipefd[i]));
    }
    for(i=0;i<number_process;i++){
        switch(pid=fork()){
            case -1:
                raler(1, "fork");
            case 0:
                if(summ==number_process){
                    CHK(close(pipefd[i][1]));
                    copy(pipefd[i][0], 1);
                    exit(EXIT_SUCCESS);
                }
                CHK(close(pipefd[i][1]));
                copy(pipefd[i][0], pipefd[i+1][1]);
                summ++;
                exit(EXIT_SUCCESS);

            default:
                CHK(close(pipefd[0][0]));
                copy(0,pipefd[0][1]);
                if ( wait (& status ) == -1)
                    raler ( 1, "wait" );
                if ( WIFEXITED ( status ))
                    printf ( " exit (% d )\n " , WEXITSTATUS ( status )) ;
                else if ( WIFSIGNALED ( status ))
                    printf ( " signal ␣ % d \n " , WTERMSIG ( status )) ;
                else
                    printf ( " autre ␣ status \n " ) ;

                
        }
    }
    return 0;
}