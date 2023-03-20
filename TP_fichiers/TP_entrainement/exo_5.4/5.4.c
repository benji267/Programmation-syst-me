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

int main(int argc, char *argv[])
{
    if(argc!=3){
        raler(0, "Usage: %s <file1> <file2>", argv[0]);
    }

    int fd1, fd2;
    CHK(fd1 = open(argv[1], O_RDONLY));
    CHK(fd2 = open(argv[2], O_WRONLY | O_TRUNC));

    struct stat *st;
    st = malloc(sizeof(struct stat));

    CHK(fstat(fd1, st));
    size_t length=st->st_size;

    char *buf;
    buf = malloc(length);

    CHK(read(fd1, buf, length));
    CHK(write(fd2, buf, length));

    free(buf);

    CHK(close(fd1));

    CHK(close(fd2));

    return 0;
}