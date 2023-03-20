#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define TAILLE_BLOC 4096
#define CHEMIN_MAX 128

#define CHK(op)		do { if ((op) == -1) raler(1, #op); } while (0)

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

struct stat_info
{
    char folder[CHEMIN_MAX + 1];
    char filename[CHEMIN_MAX + 1];
    int fd;
    off_t size;
    unsigned char *content;
};

struct stat_final
{
    char folder[CHEMIN_MAX + 1];
    char filename[CHEMIN_MAX + 1];
    int octet;
    size_t count;
    double percent;
};

int wait_for_stuff(int fd)
{
    int status = EXIT_SUCCESS;

    int nb_lus;
    struct stat_final st;
    while ((nb_lus = read(fd, &st, sizeof(struct stat_final))) > 0)
    {
        if (nb_lus != sizeof(struct stat_final))
        {
            raler(1, "read failed 10");
        }

        int n;
        char path[CHEMIN_MAX + 1];
        n = snprintf(path, CHEMIN_MAX + 1, "%s/%s", st.folder, st.filename);
        if (n < 0 || n > CHEMIN_MAX)
        {
            raler(0, "chemin trop long");
        }
        printf("%s %d %zu %6f\n", path, st.octet, st.count, st.percent);
    }

    if (nb_lus == -1)
        raler(1, "read failed 11");
    return status;
}

int make_stats(struct stat_info *st, int octet)
{
    int status = EXIT_SUCCESS;

    size_t count = 0;
    for (size_t j = 0; j < (size_t)st->size; j++)
    {
        // compare si l'octet à analyser est égal à l'octet courant
        if (octet == (int)st->content[j])
        {
            count++;
        }
    }

    double percent = (double)count / (double)st->size;
    struct stat_final stf = {
        .octet = octet, .count = count, .percent = percent};
    strncpy(stf.folder, st->folder, CHEMIN_MAX);
    strncpy(stf.filename, st->filename, CHEMIN_MAX);
    CHK(write(st->fd, &stf, sizeof(struct stat_final)));
    return status;
}

void free_stuff(void *ptr[])
{
    for (int i = 0; ptr[i]; i++)
    {
        free(ptr[i]);
    }
}

int main(int argc, char *argv[])
{
    int status = EXIT_SUCCESS;
    if (argc < 3)
    {
        raler(0, "usage: statoctet <repertoire> <octet> ...");
    }
    int nox = argc - 2; // nombre d'octets à analyser

    char *folder = argv[1];
    int *octet = malloc((argc - 2) * sizeof(int));
    if (!octet)
    {
        raler(1, "malloc failed\n");
    }

    for (int i = 0; i < nox; i++)
    {
        octet[i] = atoi(argv[i + 2]);
    }

    CHK(chdir(folder));

    // Je crée nox+1 pipes
    // le premier sera utilisé pour communiquer parent -> enfant 1
    int **pipefd = malloc((nox + 1) * sizeof(int *));
    if (!pipefd)
    {
        raler(1, "malloc failed");
    }

    for (int i = 0; i < nox + 1; i++)
    {
        pipefd[i] = malloc(2 * sizeof(int));
        if (!pipefd[i])
        {
            raler(1, "malloc failed");
        }
    }

    CHK(pipe(pipefd[nox]));

    // je crée le processeur collecteur
    switch (fork())
    {

    case -1:
        raler(1, "fork failed");
    case 0:
        CHK(close(pipefd[nox][1]));
        exit(wait_for_stuff(pipefd[nox][0]));
        break;
    }

    // je vérifie désormais les fichiers du répertoire
    DIR *dir;
    dir = opendir(".");

    if (dir == NULL)
    {
        raler(1, "opendir failed");
    }

    struct dirent *entry;
    errno = 0;

    while ((entry = readdir(dir)) != NULL)
    {

        if (entry->d_name[0] == '.')
        {
            continue;
        }

        // recupère la taille du fichier
        struct stat st;
        CHK(stat(entry->d_name, &st));

        // Si c'est un répertoire on passe au suivant
        if (!S_ISREG(st.st_mode))
        {
            continue;
        }

        // créer les pipes pour les stateurs
        for (int i = 0; i < nox; i++)
        {
            CHK(pipe(pipefd[i]));
        }

        size_t size = st.st_size;

        // on lit le fichier courant
        int fd;
        CHK(fd = open(entry->d_name, O_RDONLY));

        unsigned char *content = malloc(size);
        if (!content)
        {
            raler(1, "malloc failed");
        }

        ssize_t n;
        ssize_t offset = 0;
        while ((n = read(fd, content + offset, TAILLE_BLOC)) > 0)
        {
            offset += n;
        }

        CHK(close(fd));

        // on crée les stateurs
        for (int i = 0; i < nox; i++)
        {
            struct stat_info st;
            switch (fork())
            {

            case -1:
                raler(1, "fork failed");

            case 0:

                // les stateurs reçoivent les infos du fichier par les stateurs
                // précédents sauf pour le premier et les envoient au prochain
                // stateur sauf pour le dernier

                CHK(read(pipefd[i][0], &st, sizeof(struct stat_info)));
                if (i < nox - 1)
                {
                    CHK(write(pipefd[i + 1][1], &st, sizeof(st)));
                    // on doit fermer le coté write du pipe sinon il y auara une
                    // attente infinie
                    CHK(close(pipefd[i + 1][1]));
                }

                status = make_stats(&st, octet[i]);
                free(st.content);
                for (int i = 0; i < nox; i++)
                {
                    free(pipefd[i]);
                }
                free(pipefd);
                free(octet);
                exit(status);
                break;

            default:
                // on ferme le coté read du pipe
                if (i > 0)
                {
                    CHK(close(pipefd[i][0]));
                }
                break;
            }
        }

        // créer une structure pour passer les infos aux stateurs
        struct stat_info info;
        strncpy(info.folder, folder, CHEMIN_MAX);
        strncpy(info.filename, entry->d_name, CHEMIN_MAX);
        info.fd = pipefd[nox][1];
        info.size = size;
        info.content = content;

        // j'écris au premier stateur
        CHK(write(pipefd[0][1], &info, sizeof(info)));
        CHK(close(pipefd[0][1]));

        int wstatus;
        for (int i = 0; i < nox; i++)
        {
            CHK(wait(&wstatus));
            if (WEXITSTATUS(wstatus) != EXIT_SUCCESS)
            {
                status = EXIT_FAILURE;
            }
        }

        free(content);
        errno = 0;
    }

    if (errno != 0)
    {
        raler(1, "readdir");
    }

    // fermer le coté read et write du pipe
    CHK(close(pipefd[nox][0]));
    CHK(close(pipefd[nox][1]));

    CHK(closedir(dir));

    int wstatus;
    CHK(wait(&wstatus));
    if (WEXITSTATUS(wstatus) != EXIT_SUCCESS)
    {
        status = EXIT_FAILURE;
    }

    for (int i = 0; i < nox; i++)
    {
        free(pipefd[i]);
    }

    free(pipefd);
    free(octet);
    return status;
}


// Pour répondre à la question 1
// Le processus collecteur doit être un processus distinct des autres
// car si le processus père essayait de lire les données
// il pourrait y avoir des conflits de lecture
// et des pertes de données
// De plus le choix d'un collecteur permet aussi un fonctionnement
// de tous les stateurs en parallèle