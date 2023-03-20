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

void copier(int src, int dst){
    char buf[MAX];
    ssize_t n;
    while((n=read(src,buf,MAX))>0){
        CHK(write(dst,buf,n));
    }
}

int main(int argc, char* argv[]){
    if(argc!=2){
        raler(0,"Usage: %s <nom>",argv[0]);
    }

    int n= atoi(argv[1]);
    int tube[n-1][2];
    pid_t pid;
    for(int i=0;i<n-1;i++){
        CHK(pipe(tube[i]));
    }
    
    for(int i=0; i<n; i++){
        switch(pid=fork()){
            case -1:
                raler(1,"fork");
            case 0:
                for(int j=i; j<n-1; j++){
                    if(j==0){
                        CHK(close(tube[j][0]));
                        copier(0,tube[j][1]);
                        CHK(close(tube[j][1]));
                    }
                    else{
                        CHK(close(tube[j][0]));
                        copier(tube[j-1][0],tube[j][1]);
                        CHK(close(tube[j][1]));
                    }
                }
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }
    //le père lit les données du n-1 ième tube
    //et écrit sur la sortie standard
    CHK(close(tube[n-2][1]));
    copier(tube[n-2][0],1);
    CHK(close(tube[n-2][0]));
    for(int i=0; i<n; i++){
        CHK(wait(NULL));
    }
    return 0;
}