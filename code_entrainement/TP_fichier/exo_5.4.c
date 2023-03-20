#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define CHK(op)               \
    do {                      \
        if ((op) == -1)       \
            raler(1, #op); \
    } while (0)

noreturn void raler(int syserr, const char *msg, ...) {
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1)
        perror("");

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
    if(argc!=3){
        raler(1, "Usage: %s <directory> <pattern>", argv[0]);
    }
    int fd_to_copy, fd_copy;
    CHK(fd_to_copy = open(argv[1], O_RDONLY));
    CHK(fd_copy = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644));
    struct stat st;
    CHK(fstat(fd_to_copy, &st));
    int size=st.st_size;
    char *buffer = malloc(size);
    CHK(read(fd_to_copy, buffer, size));
    CHK(write(fd_copy, buffer, size));
    free(buffer);
    CHK(close(fd_to_copy));
    CHK(close(fd_copy));
    return 0;
}
