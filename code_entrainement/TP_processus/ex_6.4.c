#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
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

int lecture(int desc, char *buff) {
    int n = read(desc, buff, 1);
    printf("%c", *buff);
    return n;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        raler(1, "Usage: %s <file>", argv[0]);
    }
    int entree = open(argv[1], O_RDONLY);
    CHK(entree);
    pid_t pid;
    char c;
    int n;
    int raison;
    n = lecture(entree, &c);

    while (n) {
        pid = fork();

        switch (pid) {
        case -1:
            raler(1, "fork");

        case 0:
            n = lecture(entree, &c);
            exit(0);

        default:
            CHK(wait(&raison));
            n = lecture(entree, &c);
        }
    }

    int fermeture = close(entree);
    CHK(fermeture);
}