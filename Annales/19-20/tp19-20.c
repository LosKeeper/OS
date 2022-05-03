#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

/*
 * Des macros bien pratiques pour alléger les tests d'erreur.
 */

#define TCHK(op)                                                               \
    do {                                                                       \
        if ((errno = (op)) != 0)                                               \
            raler(1, #op);                                                     \
    } while (0)

#define MCHK(op)                                                               \
    do {                                                                       \
        if ((op) == NULL)                                                      \
            raler(1, #op);                                                     \
    } while (0)

// la fonction habituelle
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

// affiche un message d'erreur
noreturn void usage(void) {
    raler(0,
          "usage: prodcons <bufsize> <nconso> <niter><car> ... <niter><car>");
}

// analyse une spécification de producteur ("100x" par ex)
// arg : chaîne de caractères en entrée
// pn : adresse d'une variable pour recevoir en sortie le nombre
// pc : adresse d'une variable pour recevoir en sortie le caractère
void decode_producteur(const char *arg, long int *pn, unsigned char *pc) {
    char *end;
    long int n;

    errno = 0;
    n = strtol(arg, &end, 10);
    if (errno != 0)
        raler(1, "cannot parse '%s'", arg);
    if (arg == end)
        raler(0, "cannot parse number '%s'", arg);
    if (strlen(end) != 1)
        raler(0, "cannot parse char '%s' in '%s'", end, arg);
    if (n < 0)
        raler(0, "'%s': count must be >= 0", arg);
    *pn = n;
    *pc = *end;
}

struct thread_arg_t {
    char caractere;
    int buffersize;
    char *buffer;
    sem_t *slibre;
    sem_t *secrite;
    sem_t *sic;
    sem_t *sip;
    int *ic;
    int *ip;
};

void *producteur(void *arg) {
    struct thread_arg_t *targ = arg;
    char c = targ->caractere;
    char *buff = targ->buffer;
    int *ip = targ->ip;
    sem_t *sip = targ->sip;
    sem_t *secrite = targ->secrite;
    sem_t *slibre = targ->slibre;
    int N = targ->buffersize;

    sem_wait(slibre);
    sem_wait(sip);
    buff[*ip] = c;
    *ip = (*ip + 1) % N;
    sem_post(sip);
    sem_post(secrite);
}

/******************************************************************************
Main
******************************************************************************/

int main(int argc, char *argv[]) {
    int bufsize;
    int nconso;
    int nprod;

    int nb_first_param;
    char first_param;
    int nb_second_param;
    char second_param;

    if (argc < 4)
        usage();

    bufsize = atoi(argv[1]);
    if (bufsize <= 0)
        usage();

    nconso = atoi(argv[2]);
    if (nconso <= 0)
        usage();

    nprod = argc - 3;
    int *prod_count = malloc(nprod * sizeof(int));
    char *prod_car = malloc(nprod * sizeof(char));

    for (int i = 0; i < nprod; i++) {
        decode_producteur(argv[i + 3], &prod_count[i], &prod_car[i]);
    }

    pthread_t consos[nconso];
    pthread_t prod[nprod];

    char *buf = malloc(bufsize);
    int ic = 0;
    int ip = 0;

    sem_t slibre;
    sem_t secrite;
    sem_t sic;
    sem_t sip;
    sem_init(&slibre, 0, bufsize);
    sem_init(&secrite, 0, 0);
    sem_init(&sic, 0, 1);
    sem_init(&sip, 0, 1);

    struct thread_arg_t thread_arg;
    thread_arg.buffersize = bufsize;
    thread_arg.buffer = buf;
    thread_arg.slibre = &slibre;
    thread_arg.secrite = &secrite;
    thread_arg.sic = &sic;
    thread_arg.sip = &sip;
    thread_arg.ic = &ic;
    thread_arg.ip = &ip;

    for (int i = 0; i < nprod; i++) {
        TCHK(pthread_create(&prod[i], NULL, prod_fct, (void *)&thread_arg));
    }
    for (int i = 0; i < nconso; i++) {
        TCHK(pthread_create(&consos[i], NULL, conso_fct, (void *)&thread_arg));
    }

    for (int i = 0; i < nprod; i++) {
        TCHK(pthread_join(prod[i], NULL));
    }
    for (int i = 0; i < nconso; i++) {
        TCHK(pthread_join(consos[i], NULL));
    }

    free(buf);

    exit(0);
}
