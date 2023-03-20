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

int _isatty(const char *fd){
    struct stat st;
    CHK(stat(fd, &st));
    if(S_ISCHR(st.st_mode)){
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]){
    if(argc == 1){
        raler(0, "Usage: %s <file>", argv[0]);
    }
    if(_isatty(argv[1])){
        printf("Le fichier est un terminal\n");
    }
    else{
        printf("Le fichier n'est pas un terminal\n");
    }
    return 0;
}