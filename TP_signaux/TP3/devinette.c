#include <stdarg.h>
#include <unistd.h>
#include <stdnoreturn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define MAX 99
#define MIN 1

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

struct guesser{
    pid_t pid;
    int guess_number;
    int count;
};

int getrandint(int a, int b){
    return rand() % (b - a) + a;
}

volatile sig_atomic_t sigusr1 = 0;
volatile sig_atomic_t sigusr2 = 0;
volatile sig_atomic_t sigterm = 0;

void handler_usr(int sig){
    switch(sig){
        case SIGUSR1:
            sigusr1 = 1;
            break;
        case SIGUSR2:
            sigusr2 = 1;
            break;
    }
    return;
}

void handler_fin(int sig){
    (void) sig;
    sigterm = 1;
    return;
}

void preparer(void){
    struct sigaction sa_usr, sa_term;
    sa_usr.sa_handler = handler_usr;
    sa_term.sa_handler = handler_fin;
    sa_usr.sa_flags = 0;
    sa_term.sa_flags = 0;
    CHK(sigemptyset(&sa_usr.sa_mask));
    CHK(sigemptyset(&sa_term.sa_mask));
    CHK(sigaction(SIGUSR1, &sa_usr, NULL));
    CHK(sigaction(SIGUSR2, &sa_usr, NULL));
    CHK(sigaction(SIGTERM, &sa_term, NULL));
}

void fils(int tube){
    int guess_number;
    struct guesser guesser;
    guesser.pid = getpid();
    guesser.count = 0;

    sigset_t mask, empty_mask;
    CHK(sigemptyset(&mask));
    CHK(sigemptyset(&empty_mask));
    CHK(sigaddset(&mask, SIGUSR1));
    CHK(sigaddset(&mask, SIGUSR2));
    CHK(sigaddset(&mask, SIGTERM));
    CHK(sigprocmask(SIG_BLOCK, &mask, NULL));

    int new_min = MIN;
    int new_max = MAX;

    //j'utilise l'horloge de mon système
    //afin d'avoir des séquences plus aléatoires avec rand
    
    srand(time(NULL) + getpid());

    while(sigterm != 1){
        
        if(sigusr1 == 1){
            sigusr1 = 0;
            //j'actualise à chaque essai le max pour converger
            new_max= guess_number;        
        }

        if(sigusr2 == 1){
            sigusr2 = 0;
            //j'actualise à chaque essai le min pour converger
            new_min = guess_number;
        }

        guess_number = getrandint(new_min, new_max);
        guesser.guess_number = guess_number;
        printf("Je suis le fils %d et je propose : %d.\n", getpid(), guess_number);
        fflush(stdout);
        guesser.count++;
        CHK(write(tube, &guesser, sizeof(guesser)));
        sigsuspend(&empty_mask);
    }
    return;
}

pid_t lecture(int val, int tube) {
    ssize_t bytes_number;
    pid_t winner;
    struct guesser guesser;
    int end_child=0;

    while ((bytes_number = read(tube, &guesser, sizeof(guesser))) > 0) {

        //si on a un gagnant on ne lit plus les propositions des autres processus
        if(end_child == 1){
            printf("Trop tard nous avons déjà un gagnant.\n");
            printf("Arrêt du processus %d.\n", guesser.pid);
            fflush(stdout);
            CHK(kill(guesser.pid, SIGTERM));
            
        }
        else{
            if (guesser.guess_number > val) {
                printf("%d propose %d mais le nombre secret est plus petit. Essais : %d.\n", 
                guesser.pid, guesser.guess_number, guesser.count);
                fflush(stdout);
                CHK(kill(guesser.pid, SIGUSR1));
            }
            else if (guesser.guess_number < val) {
                printf("%d propose %d mais le nombre secret est plus grand. Essais : %d.\n", 
                guesser.pid, guesser.guess_number, guesser.count);
                fflush(stdout);
                CHK(kill(guesser.pid, SIGUSR2));
            }
            else {
                winner = guesser.pid;
                end_child = 1;
                CHK(kill(guesser.pid, SIGTERM));
            }
        }
    }
    return winner;
}


void attendre_fils(int n){
    int status;
    while(n > 0){
        CHK(wait(&status));

        if(WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS){
            raler(1, "un fils s'est mal terminé.");
        }
        n--;
    }
}

// fonction de test des arguments

void tests(int argc, char* argv[]){
    if(argc != 3){
        raler(1, "usage : devinette <nb_secret> <nb_de_processus>");
    }

    if(atoi(argv[1]) > MAX || atoi(argv[1]) < MIN){
        raler(1, "le nombre secret doit être compris entre 1 et 99");
    }

    if(atoi(argv[2]) < 1){
        raler(1, "le nombre de processus doit être supérieur à 0");
    }
}

int main(int argc, char* argv[]){
    tests(argc, argv);

    int tube[2];
    CHK(pipe(tube));

    preparer();

    //je crée les différents fils
    for(int i = 0; i < atoi(argv[2]); i++){
        switch(fork()){
            case -1:
                raler(1, "fork");
            case 0:
                CHK(close(tube[0]));
                fils(tube[1]);
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }

    CHK(close(tube[1]));

    pid_t winner = lecture(atoi(argv[1]), tube[0]);

    CHK(close(tube[0]));

    attendre_fils(atoi(argv[2]));

    printf("Félicitations au gagnant.\n");
    printf("C'est le processus %d avec le nombre %d.\n", winner, atoi(argv[1]));

    return 0;
}