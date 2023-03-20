#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024    
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

// read a character from standard input
// buffer version
int _getchar(void){
    static ssize_t size_buf = 0;
    static ssize_t index_buf = 0;
    static unsigned char buf[BUFFER_SIZE];
    if(index_buf >= size_buf){
        CHK(size_buf = read(STDIN_FILENO, buf, BUFFER_SIZE));
        if(size_buf == 0){
            return EOF;
        }
        index_buf = 0;
    }
    return buf[index_buf++];
}

int main(int argc, char const *argv[])
{   
    int c;
    while((c = _getchar()) != EOF){
        CHK(putchar(c));
    }
    return 0;
}