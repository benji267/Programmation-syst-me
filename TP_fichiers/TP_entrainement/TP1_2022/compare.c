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
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end (ap);

    if (syserr == 1)
        perror ("");

    exit (EXIT_FAILURE);
}


int main(int argc, char* argv[]){

    if(argc!=3){
        perror("Wrong number of arguments \n");
        return 1;
    }

    long int byte, line;

    int desc1, desc2;

    CHK(desc1 = open(argv[1], O_RDONLY));

    CHK(desc2 = open(argv[2], O_RDONLY));

    struct stat st1, st2;

    CHK(fstat(desc1, &st1));

    CHK(fstat(desc2, &st2));

    if(st1.st_size==0 && st2.st_size!=0){
        fprintf(stderr, "EOF on %s which is empty \n", argv[1]);
        CHK(close(desc1));
        CHK(close(desc2));
        return 1;
        fflush(stdout);
    }

    if(st2.st_size==0 && st1.st_size!=0){
        fprintf(stderr, "EOF on %s which is empty \n", argv[2]);
        CHK(close(desc1));
        CHK(close(desc2));
        return 1;
        fflush(stdout);
    }

    char c1, c2;

    byte = 0;
    line = 1;
    
    while(read(desc1, &c1, 1)>0 && read(desc2, &c2, 1)>0){
        byte++;

        if(c1=='\n'){
            line++;
        }
         
        if(c1==EOF && c2!=EOF){
            fprintf(stderr, "EOF on %s after byte %ld, line %ld \n", argv[1], byte, line);
            CHK(close(desc1));
            CHK(close(desc2));
            return 1;
            fflush(stdout);
        }
        if(c1!=EOF && c2==EOF){
            fprintf(stderr, "EOF on %s after byte %ld, line %ld \n", argv[2], byte, line);
            CHK(close(desc1));
            CHK(close(desc2));
            return 1;
            fflush(stdout);
        }

        if(c1!=c2){
            fprintf(stderr, "%s %s differ: byte %ld, line %ld\n", argv[1], argv[2], byte, line);
            CHK(close(desc1));
            CHK(close(desc2));
            return 1;
            fflush(stdout);
        } 
    }
    
    CHK(close(desc1));
    CHK(close(desc2));
    return 0;
}