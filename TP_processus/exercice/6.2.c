#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


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
    if(argc == 1){
        raler(0, "Usage: %s <file>", argv[0]);
    }
    
    int nmb_processus=atoi(argv[1]);
    
    int *tab_pid = malloc(nmb_processus*sizeof(int));
    int raison;
    for(int i=0; i<nmb_processus; i++){
        switch(tab_pid[i] = fork()){
            case -1:
                raler(1, "fork");
            case 0:
                exit(i);
            
    }
    }
    for(int i=0; i<nmb_processus; i++){
        printf("pid ␣ %d ␣ : ␣ " , tab_pid[i]);
        CHK(wait(&raison));
        if ( WIFEXITED ( raison )){
            printf ( " exit (% d )\n " , WEXITSTATUS ( raison )) ;
        }
        else if ( WIFSIGNALED ( raison )){
            printf ( " signal ␣ % d \n " , WTERMSIG ( raison )) ;
        }
        else{
            printf ( " autre ␣ raison \n " ) ;
        }
    

    }
}
