#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdnoreturn.h>

void parcours(char* chemin);

#define CHK(op) do { if ((op) == -1) raler (1, #op); } while (0)
#define CHKN(op)	do { if ((op) == NULL) raler(1, #op); } while (0)


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
#define CHEMIN_MAX 512


void execute_wc(char * fichier){
    int status;
    int fd;
    pid_t pid;
    switch(pid=fork()){
        case -1:
            raler(1, "fork");
        case 0:
            CHK(fd = open(fichier, O_RDONLY));
            CHK(dup2(fd, 0));
            CHK(close(fd));
            execlp("wc", "wc", "-l", NULL);
            raler(1, "exec");
        default:
    }
}


void traite(char* nom){
    pid_t pid;
    switch(pid=fork()){
        case -1:
            raler(1, "fork");
        case 0:
            struct stat statbuf;
            CHK(lstat(nom, &statbuf));

            if(S_ISREG(statbuf.st_mode)){
                execute_wc(nom);
            }
            else if(S_ISDIR(statbuf.st_mode)){
                parcours(nom);
                exit(EXIT_SUCCESS); 
            }
        default:
    }
    //j'attend pas les fils car ce sera fais avec la fonction attend
}



void attend(int n){
    int status;
    for(int i=0; i<n; i++){
        CHK(wait(&status));
        if( !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS ) {
            raler(0, "Erreur exécution\n");
        }
    }
}

void parcours(char* chemin){
    int nb_fils=0;
    DIR *dir;
    CHKN(dir=opendir(chemin));
    struct dirent *d;
    errno=0;
    while((d=readdir(dir))!=NULL){
        if (strcmp (d->d_name, ".") != 0 && strcmp (d->d_name, "..") != 0){
            char chemin_fichier[CHEMIN_MAX+1];
            snprintf(chemin_fichier, CHEMIN_MAX+1, "%s/%s", chemin, d->d_name);
            traite(chemin_fichier);
            nb_fils++;
        }
        errno=0;
    }
    if(errno!=0){
        raler(1, "readdir");
    }
    attend(nb_fils);
}

int main(int argc, char* argv[]){
    if(argc!=2){
        raler(0, "Nombre d'arguments incorrect\n");
    }
    parcours(argv[1]);
    return 0;
}