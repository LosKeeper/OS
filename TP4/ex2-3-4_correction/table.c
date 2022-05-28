#include <fcntl.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Compiler avec -DINTERBLOCAGE pour voir l'interblocage se produire
 *	(visible seulement avec un très grand nombre de repas, par ex 10000)
 * Compiler avec -DTERMINAISON pour que table attende que le dernier
 *	philosophe ait quitté la table
 * Compiler sans -D pour une version sans interblocage et sans attente
 *	de terminaison
 */

/*
 * Il faudrait mettre la définition dans un fichier .h séparé pour mutualiser
 * avec philo.c
 */

#define MEMNAME "/table"
struct mem {
    // partie fixe : unique pour tous les philosophes
    int nphilo;
    int narrives;
    sem_t mtx; // exclusion mutuelle (init = 1)
#if defined(TERMINAISON)
    int ntermines; // nb de philosophes terminés
    sem_t fin;     // attente de la fin (init = 0)
#endif

    // partie variable : il y a un élément par philosophe (ou plus
    // exactement par baguette)
    // Note : les tableaux sans dimension sont autorisés à la fin des
    // structures, sizeof renvoie alors la taille sans cet élément
    sem_t tabsem[]; // baguette (init = 1)
};

#define CHK(op)            \
    do {                   \
        if ((op) == -1)    \
            raler(1, #op); \
    } while (0)

noreturn void raler(int syserr, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr)
        perror("");

    exit(1);
}

void usage(char *prog) { raler(0, "usage: %s nphilo", prog); }

int main(int argc, char *argv[]) {
    int nphilo, fd, i;
    off_t sz;
    struct mem *m;

    if (argc != 2)
        usage(argv[0]);

    nphilo = atoi(argv[1]);
    if (nphilo <= 0)
        usage(argv[0]);

    sz = sizeof(struct mem) + nphilo * sizeof(sem_t);

    CHK(fd = shm_open(MEMNAME, O_RDWR | O_CREAT | O_TRUNC, 0666));
    CHK(ftruncate(fd, sz));
    m = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (m == MAP_FAILED)
        raler(1, "cannot mmap");
    CHK(close(fd)); // on n'a plus besoin de l'ouverture

    m->nphilo = nphilo;
    m->narrives = 0;
    CHK(sem_init(&m->mtx, 1, 1));
#if defined(TERMINAISON)
    m->ntermines = 0;
    CHK(sem_init(&m->fin, 1, 0));
#endif

    for (i = 0; i < nphilo; i++)
        CHK(sem_init(&m->tabsem[i], 1, 1));

#if defined(TERMINAISON)
    // attendre la fin : le dernier philosophe qui quitte la table
    // doit nous réveiller
    CHK(sem_wait(&m->fin));

    CHK(munmap(m, sz));
    CHK(shm_unlink(MEMNAME));
#endif

    exit(0);
}
