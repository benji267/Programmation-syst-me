#include "chaine.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "chaine.h"

void _cd(char *chemin)
{
    if (chemin == NULL)
    {
        chemin = getenv("HOME");
        if (chemin == NULL)
        {
            fprintf(stderr, "cd: variable HOME non définie\n");
        }
    }
    else
    {
        if(chdir(chemin)==-1){
            fprintf(stderr, "cd: %s: %s\n", chemin, strerror(errno));
        }
    }
    
}

void _umask(char * masque)
{
    mode_t current_mask = umask(0);
    if (masque)
    {
        umask(CONVERT(masque));
    }
    else
    {
        umask(current_mask);
        PRINTOCT(current_mask);
    }
}

void _print()
{
    printf("%d", errno);
}

void _exit_()
{
    
    exit(EXIT_SUCCESS);
}

void attendre_fils(void)
{
    int raison;

    CHK(wait(&raison));
    if (!(WIFEXITED(raison) && WEXITSTATUS(raison) == 0))
    {
    }

    return;
}

int main(void)
{
    ssize_t size;
    char buffer[1024+1];
    int end = 0;
    while (!end)
    {
        CHK(size = read(0, buffer, 1024));
        if (size == 0)
        {
            end = 1;
        }

    
    
    char** tab = decompose(buffer, size);
    if (strcmp(tab[0], "cd") == 0)
    {
        _cd(tab[1]);
    }
    else if (strcmp(tab[0], "umask") == 0)
    {
        _umask(tab[1]);
    }
    else if (strcmp(tab[0], "print") == 0)
    {
        _print();
    }
    else if (strcmp(tab[0], "exit") == 0)
    {
        free_tab(tab);
        _exit_();
    }
    else
    {
        pid_t pid;
        switch (pid = fork())
        {
        case -1:
            raler(1, "fork");
            break;
        case 0:
            execvp(tab[0], tab);
            perror("execvp");
            free_tab(tab);
            exit(EXIT_FAILURE);
        default:
            attendre_fils();
        }
    }
    free_tab(tab);
    
    }
    return 0;
}