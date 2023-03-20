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


void f(int sig){
    if(sig != SIGINT)
        raler(0, "Signal reçu non géré !\n");
    char* MSG = "\nSignal reçu !\n";
    write(1, MSG, strlen(MSG));
}

int main(void){
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = f;
    CHK(sigemptyset(&sa.sa_mask));
    CHK(sigaction(SIGINT, &sa, NULL));
    sleep(3);
    sigset_t new, old;
    CHK(sigemptyset(&new));
    CHK(sigaddset(&new, SIGINT));
    CHK(sigprocmask(SIG_BLOCK, &new, &old));
    sleep(3);
    CHK(sigprocmask(SIG_SETMASK, &old, NULL));
    sleep(3);
    return 0;
}