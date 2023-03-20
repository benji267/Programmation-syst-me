#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFF_SIZE 1024

typedef struct f{
    int desc;
    char* buf;
}FICHIER;

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



FICHIER *my_open(const char* pathname, const char* mode){
    
    int descriptor;
    struct f file;

    if(&mode[0]=="r"){
        descriptor=open(pathname, O_RDONLY);
        file.desc=descriptor;
        return &file;
    }

    if(&mode[0]=="w"){
        descriptor=open(pathname, O_RDWR);
        file.desc=descriptor;
        return &file;
    }
    return NULL;
}





char my_getc(FILE *stream){

    ssize_t index=0;

    ssize_t size=0;
    struct f file;
    char buffer[BUFF_SIZE];

    while(index>=size){
        
        if(read(0, &buffer, BUFF_SIZE)<=0){
            return EOF;
        }
        index=0;
    }
    file.buf=buffer;
    return buffer[0];
}

void my_putc(char* Char, FILE* stream){


    struct f* file;

    char buffer[BUFF_SIZE]=file->buf;
    ssize_t index=1;

    int descriptor=file->desc;

    while(index){

        while(write(descriptor,buffer,))
    }
    
}
