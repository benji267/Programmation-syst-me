#include <stdarg.h>
#include <unistd.h>
#include <stdnoreturn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

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

volatile sig_atomic_t cpt = 0;

void f(){
    cpt++;
}

int main(void){
    struct sigaction s;
    s.sa_handler = f;
    s.sa_flags = 0;
    CHK(sigemptyset(&s.sa_mask));
    CHK(sigaction(SIGUSR1, &s, NULL));

    pid_t pid;
    switch(pid = fork()){
        case -1:
            raler(1, "fork");

        case 0:
            for(long int i=0; i<1000000;i++){
                CHK(kill(getppid(), SIGUSR1));
            }
            break; 
        default:
            break;
    }

    while((wait(NULL) != -1) && (errno != EINTR)){
        sleep(1);
    }
    printf("cpt = %d !\n", cpt);
    return 0;
}