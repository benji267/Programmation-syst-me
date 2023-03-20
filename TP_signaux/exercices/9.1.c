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

volatile sig_atomic_t sigint = 0;

void f(){
    sigint++;
    printf("\nSIGINT %d fois !\n", sigint);
}

int main(void){
    struct sigaction s;
    s.sa_handler = f;
    s.sa_flags = 0;
    sigemptyset(&s.sa_mask);
    CHK(sigaction(SIGINT, &s, NULL));
    while(sigint!=5){
        sleep(1);
    }
    return 0;
}