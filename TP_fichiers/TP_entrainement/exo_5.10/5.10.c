#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

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

int main(int argc, char* argv[]){

    if(argc != 2){
        raler(0, "Usage: %s <file>", argv[0]);
    }


    DIR *dir = opendir(argv[1]);
    if(dir == NULL){
        raler(0, "Error: %s is not a directory", argv[1]);
    }
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            printf("%s\n", entry->d_name);
        }
        errno=0;      
    }
    if(errno != 0){
        raler(0, "Error: %s", strerror(errno));
    }

    CHK(closedir(dir));

    return 0;
}
    