#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

#define MAXLEN 1000 // longueur maximum d'une ligne

#define NCHK(op)                                                               \
    do {                                                                       \
        if ((op) == NULL) {                                                    \
            raler(1, #op);                                                     \
        }                                                                      \
    } while (0)
#define TCHK(op)                                                               \
    do {                                                                       \
        if ((errno = (op)) > 0) {                                              \
            raler(1, #op);                                                     \
        }                                                                      \
    } while (0)

struct arg {
    char *fichier;      // fichier que le thread doit lire
    char ligne[MAXLEN]; // chaque ligne lue par le thread
    int *termine;       // indique si le thread est terminé
    sem_t *sem;         // semaphore pour la manipulation du fichier
};

noreturn void raler(int syserr, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr)
        perror("");

    exit(EXIT_FAILURE);
}

void *f(void *arg) {
    struct arg *a = arg;
    char ligne[MAXLEN]; // y compris le \0 de fin de chaîne
    FILE *fp;

    NCHK(fp = fopen(a->fichier, "r"));
    // lire la ligne dans la variable locale du thread, pendant que le thread
    // principal affiche la ligne partagée

    while (fgets(ligne, sizeof ligne, fp) != NULL) {
        // recopier la ligne lue dans la variable partagée
        sem_post(a->sem);
        sem_wait(a->sem);
        strcpy(a->ligne, ligne);
        }

    if (fclose(fp) == EOF)
        raler(1, "fclose %s", a->fichier);

    *a->termine = 1;

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t id;
    struct arg arg;
    int termine = 0;

    sem_t sem;
    sem_init(&sem, 0, 1);

    if (argc != 2)
        raler(0, "usage: %s fichier", argv[0]);

    arg.fichier = argv[1];
    arg.sem = &sem;
    arg.termine = &termine;

    TCHK(pthread_create(&id, NULL, f, &arg));

    while (!termine) {
        sem_wait(&sem);
        printf("%s", arg.ligne);
        fflush(stdout);
        sem_post(&sem);
    }

    TCHK(pthread_join(id, NULL));

    sem_destroy(&sem);

    exit(0);
}
