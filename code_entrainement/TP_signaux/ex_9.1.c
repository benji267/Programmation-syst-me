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


volatile __sig_atomic_t cpt=0;

void f(int sig)
{
    if(sig == SIGINT){
        cpt++;
        printf("\nsignal SIGINT reçue. Le compteur est à : cpt=%d\n", cpt);
    }
}

int main(){
    struct sigaction sa;
    sa.sa_handler = f;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    CHK(sigaction(SIGINT, &sa, NULL));
    while(cpt!=5){
        sleep(1);
    }
    return 0;
}