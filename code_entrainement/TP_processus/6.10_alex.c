#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE_BUF 1024
#define PATH_MAX 4096
#define DELIMITER ' '

#define CHK(op)                                                                \
    do                                                                         \
    {                                                                          \
        if ((op) == -1)                                                        \
            raler(1, #op);                                                     \
    } while (0)
#define CHK_NUL(op)                                                            \
    do                                                                         \
    {                                                                          \
        if ((op) == NULL)                                                      \
            raler(1, #op);                                                     \
    } while (0)

#define CHK_ERRNO(void)                                                        \
    do                                                                         \
    {                                                                          \
        if (errno != 0)                                                        \
            raler(1, "Erreur errno");                                          \
    } while (0)

noreturn void raler(int syserr, const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1)
        perror("");

    exit(EXIT_FAILURE);
}

char **separe(char *chaine, char delimiter)
{
    // on compte le nombre de répertoire contenu dans 'chaine'
    int i = 0, rep = 1;
    while (chaine[i] != '\0')
        if (chaine[i++] == delimiter)
            rep++;

    // création du tableau qui contiendra les noms des répertoires
    char **tab_rep;
    if ((tab_rep = malloc((rep + 1) * sizeof *tab_rep)) == NULL)
        raler(1, "malloc");
    tab_rep[rep] = NULL;

    i = 0;
    for (int j = 0; j < rep;)
    {
        if (chaine[i] == delimiter || chaine[i] == '\0')
        { // on a le nom complet d'un répertoire
            chaine[i] = '\0';

            if ((tab_rep[j] = malloc(strlen(chaine) + 1)) == NULL)
                raler(1, "malloc tab_rep");
            strcpy(tab_rep[j], chaine);

            chaine = &chaine[i] + 1; // on passe au répertoire suivant
            i = 0;
            j++;
        }
        else
            i++;
    }

    return tab_rep;
}

char *check_path(char *path, const char *to_find)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
        raler(0, "cannot open directory %s", path);
    struct stat st;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char real_path[SIZE_BUF];
        sprintf(real_path, "%s/%s", path, entry->d_name);
        CHK(lstat(real_path, &st));
        if (S_ISREG(st.st_mode) && strcmp(entry->d_name, to_find) == 0)
        {
            char *res = malloc(PATH_MAX + 1 * sizeof(char));
            memset(res, 0, PATH_MAX + 1);
            snprintf(res, PATH_MAX, "%s", real_path);
            CHK(closedir(dir));
            return res;
        }
        else if (S_ISDIR(st.st_mode))
        {
            check_path(real_path, to_find);
        }
    }
    CHK(closedir(dir));
    return NULL;
}

void free_2D_array(char **to_free)
{
    for (size_t i = 0; to_free[i] != NULL; i++)
        free(to_free[i]);
    free(to_free);
}

void fils(char **paths, char *buf, char **args)
{
    for (size_t i = 0; paths[i] != NULL; i++)
    {
        char *res = check_path(paths[i], buf);
        if (res != NULL)
        {
            char buf_path_cmd[SIZE_BUF];
            snprintf(buf_path_cmd, SIZE_BUF, "%s/%s", paths[i], args[0]);
            free(res);
            execv(buf_path_cmd, args);
            break;
        }
        free(res);
    }
    raler(0, "command not found");
}

int main(void)
{
    char buf[SIZE_BUF];
    while (fgets(buf, SIZE_BUF, stdin))
    {
        buf[strcspn(buf, "\n")] = 0;
        char **args = separe(buf, DELIMITER);
        char *path_user = getenv("PATH");
        if (path_user == NULL)
            raler(0, "path_var null!");
        char *tmp = strdup(path_user);
        // separe modifie le pointeur donner en entrée
        // crade de faire comme ça mais pas envie de passer trop de temps
        // dessusddddddddddddddddd
        char **paths = separe(tmp, ':');
        free(tmp);
        switch (fork())
        {
        case -1:
            raler(1, "fork");
            break;
        case 0:
            fils(paths, buf, args);
            break;
        default:
            int raison;
            CHK(wait(&raison));
            if (WIFEXITED(raison) && WEXITSTATUS(raison) == EXIT_FAILURE)
            {
                raler(0, "error\n");
            }
            break;
        }
        free_2D_array(args);
        free_2D_array(paths);
    }
    return 0;
}
