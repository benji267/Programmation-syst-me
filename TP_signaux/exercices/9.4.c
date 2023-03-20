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

volatile sig_atomic_t stop = 0;
volatile sig_atomic_t time = 0;

void f(int sig){
    (void) sig;
    stop = 1;
}

void traite(int time){
    printf("Je suis le fils, time: %d\n", time);
    time++;
}

int main(void){
    struct sigaction sa;
    sa.sa_handler = f;
    sa.sa_flags = 0;
    CHK(sigemptyset(&sa.sa_mask));
    CHK(sigaction(SIGUSR1, &sa, NULL));
    CHK(sigemptyset(&sa.sa_mask));
    sa.sa_handler = traite;
    CHK(sigaction(SIGALRM, &sa, NULL));
    pid_t pid;
    switch(pid=fork()){
        case -1:
            raler(1, "fork");
        case 0:
            while(!stop){
                alarm(1);
                traite(time);
            }
            printf("Je suis le fils et je meurs\n");
            exit(EXIT_SUCCESS);
        default:
            break;
    }
    while(time!=60){
        sleep(1);
    }
    CHK(kill(pid, SIGUSR1));
    int status;
    CHK(wait(&status));

    if(WIFEXITED(status)){
        printf("Le fils s'est terminé normalement avec le code %d et le père aussi !\n", WEXITSTATUS(status));
    }
    return 0;

}