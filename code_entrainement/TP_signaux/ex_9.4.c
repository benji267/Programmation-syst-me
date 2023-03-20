#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <signal.h>
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

volatile __sig_atomic_t traite=0;
volatile __sig_atomic_t sig_usr_1=0;

void _traite(){
    traite++;
    printf("\n The count is around : %d\n", traite);
}

void f(int sig){
    if(sig == SIGUSR1){
        sig_usr_1++;
    }
}

int main(){
    pid_t pid;
    pid_t pid_fils;
    sigset_t mask;
    struct sigaction sa;
    sa.sa_handler = f;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    CHK(sigaction(SIGUSR1, &sa, NULL));
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    switch(pid=fork()){
        case -1:
            raler(1, "fork");
        case 0:
            pid_fils=getpid();
            while(traite!=60){
                _traite();
                sleep(1);
            }
            if(sig_usr_1==0){
                printf("\nThe process doesn't get any signal SIGUSR1\n");
                sigsuspend(&mask);
            }
            printf("Message received father, I stop\n");
            break;
        default:
            break;
    }
    alarm(60);
    printf("It makes 60 seconds. You have to stop son.\n");
    kill(SIGUSR1, pid_fils);
    return 0;
    }