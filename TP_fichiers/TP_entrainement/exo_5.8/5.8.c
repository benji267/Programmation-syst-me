#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int main(int argc, char *argv[]){

    if(argc!=2){
        printf("Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd;
    CHK(fd = open(argv[1], O_RDONLY));

    struct stat st;

    CHK(stat(argv[1], &st));

    printf("TYPE OF FILE: ");

    switch(st.st_mode & S_IFMT){
        case S_IFREG:
            printf("Regular file\n");
            break;
        case S_IFDIR:
            printf("Directory\n");
            break;
        case S_IFCHR:
            printf("Character device\n");
            break;
        case S_IFBLK:
            printf("Block device\n");
            break;
        case S_IFIFO:
            printf("FIFO\n");
            break;
        case S_IFLNK:
            printf("Symbolic link\n");
            break;
        case S_IFSOCK:
            printf("Socket\n");
            break;
        default:
            printf("Unknown\n");
            break;
    }

    printf("PERMISSIONS: %o\n", st.st_mode & 0777);

    return 0;

    
    

}