#include <stdio.h>
#include <string.h>
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

typedef struct _FICHIER{
    int fd;
    unsigned char* buff;
    size_t bufsize;
    size_t pos;
    char mode;
}FICHIER;

FICHIER* my_open(const char* filename, const char* mode){

    FICHIER* f = malloc(sizeof(FICHIER));

    f->buff = (unsigned char*)malloc(1024);

    f->pos=0;

    if(f==NULL){
        return NULL;
    }
    if(strcmp(mode, "r")==0){
        f->fd=open(filename, O_RDONLY);
        f->mode='r';
        f->bufsize=1024;
        return f;
    }
    
    else if(strcmp(mode, "w")==0){
        f->fd=open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        f->mode='w';
        f->bufsize=1024;
        return f;
    }
    return NULL;
}

int my_getc(FICHIER* f){
    
    if(f->pos >= f->bufsize){
        f->pos=0;
        int n=read(f->fd, f->buff, 1024);
        if(n==0){
            return EOF;
        }
        if(n==-1){
            return -1;
        }
    }
    return f->buff[f->pos++];
}

int my_putc(int c, FICHIER* f){
    f->buff[f->pos++]=c;
    if(f->pos >= f->bufsize){
        f->pos=0;
        int n=write(f->fd, f->buff, 1024);
        if(n==-1){
            return -1;
        }
    }
    return c;
}

int my_close(FICHIER* f){
    int fermeture=close(f->fd);

    free(f->buff);

    free(f);

    return fermeture;
}

int main(){
    FICHIER* f1=my_open("toto", "r");
    FICHIER* f2=my_open("tata", "r");

    int c1, c2;

    while(my_getc(f1) != EOF && my_getc(f2) != EOF){
        c1=my_getc(f1);
        c2=my_getc(f2);
        my_putc(c1, f1);
        my_putc(c2, f2);
    }
    my_close(f1);
    my_close(f2);
    return 0;

    
}