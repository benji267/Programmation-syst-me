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
#define BUFFER_SIZE 1024

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


int test_palyndrome(char* buf, size_t size){
    size_t i;
    if(size%2 == 0){
        for(i = 0; i < size/2; i++){
            if(buf[i] != buf[size-i-1]){
                return 1;
            }
        }
    }
    size_t middle=size/2;
    for(i = 0; i < middle; i++){
        if(buf[i] != buf[size-i-1]){
            return 1;
        }
    }
    return 0;
}

int main(int argc,char* argv[]){
    if (argc != 1) {
        raler(0, "Usage: %s <file>", argv[0]);
    }

    char* buf=malloc(BUFFER_SIZE);
    int size;

    CHK(size=read(0, buf, BUFFER_SIZE));

    if(test_palyndrome(buf, size) == 1){
        printf("C'est un palyndrome.\n");
    }
    else{
        printf("Ce n'est pas un palyndrome.\n");
    }

    free(buf);
    return 0;
}