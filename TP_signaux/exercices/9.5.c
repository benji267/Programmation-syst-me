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

void f(int sig){
    int toto;
    CHK(toto = open("toto", O_WRONLY | O_APPEND/ O_CREAT, 0666));
    switch(sig){
        case SIGINT:
            time_t curtime;
            CHK(time(&curtime));
            CHK(write(toto, ctime(&curtime), 24));
            CHK(write(toto, ", ", 2));
            char buf[10];
            snprintf(buf, 10, "%d", cpt);
            CHK(write(toto, "compteur = ", 11));
            CHK(write(toto, buf, strlen(buf)));
            CHK(write(toto, " ", 1));
            CHK(close(toto));
            break;

        case SIGTERM:
            CHK(write(toto, "fin", 3));
            CHK(close(toto));
            exit(EXIT_SUCCESS);
            break;
    }
}


int main(void){
    struct sigaction s;
    memset(&s, 0, sizeof(s));

    s.sa_handler = f;
    s.sa_flags = 0;
    CHK(sigemptyset(&s.sa_mask));
    CHK(sigaddset(&s.sa_mask, SIGTERM));
    CHK(sigaction(SIGINT, &s, NULL));
    CHK(sigemptyset(&s.sa_mask));
    CHK(sigaddset(&s.sa_mask, SIGINT));
    CHK(sigaction(SIGTERM, &s, NULL));

    while(1){
        sleep(1);
        cpt++;
    }
    return 0;
}

