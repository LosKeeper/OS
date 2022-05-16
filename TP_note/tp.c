#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

#define CHK(op)                                                                \
    do {                                                                       \
        if ((op) == -1)                                                        \
            raler(1, #op);                                                     \
    } while (0)
#define NCHK(op)                                                               \
    do {                                                                       \
        if ((op) == NULL)                                                      \
            raler(1, #op);                                                     \
    } while (0)
#define TCHK(op)                                                               \
    do {                                                                       \
        if ((errno = (op)) > 0)                                                \
            raler(1, #op);                                                     \
    } while (0)

#define MAXNOM 10

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

struct arg {
    char nom[MAXNOM]; // le nom du client
    int duree;        // durée de la course en ms
    int *occupes;     // le tableau des livreurs occupés
    // TODO à compléter

    int nb_livreurs;
    pthread_cond_t *cond_emetteur;
    pthread_cond_t *cond_livreur;
    pthread_mutex_t *mutex_livreur;
    pthread_mutex_t *mutex_emetteur;
};

void *livreur(void *arg) {
    struct arg *a = arg;

    for (;;) {
        // TODO attendre une course
        TCHK(pthread_cond_wait(a->cond_livreur, a->mutex_livreur));
        TCHK(pthread_mutex_unlock(a->mutex_livreur));
        TCHK(pthread_mutex_lock(a->mutex_livreur));
        while (a->nom[0] == '\0')
            ; // attente active... beuark ! => il faut une
              // attente passive

        // détecter la terminaison
        if (a->duree < 0)
            break;

        TCHK(pthread_mutex_unlock(a->mutex_livreur));

        // TODO modifier le message pour inclure le numéro du livreur
        printf("Livreur %d : client %s pendant %d ms\n", a->nb_livreurs, a->nom,
               a->duree);

        usleep(a->duree * 1000); // durée de la course en micro-secondes

        // TODO réveiller le thread principal quand on a fini
        TCHK(pthread_mutex_lock(a->mutex_emetteur));
        a->occupes[a->nb_livreurs] = 0;
        TCHK(pthread_mutex_unlock(a->mutex_emetteur));
        TCHK(pthread_cond_signal(a->cond_emetteur));
    }

    return NULL;
}

int trouver_livreur_pret(int *occupes, int nthreads) {
    int i;

    for (i = 0; i < nthreads; i++)
        if (!occupes[i])
            break;

    return (i < nthreads) ? i : -1; // -1 <=> aucun livreur prêt
}

int main(int argc, char *argv[]) {
    int nthreads;     // nb de livreurs
    pthread_t *tid;   // tableau des identificateurs de threads
    struct arg *targ; // tableau des arguments
    int *occupes;     // tableau des livreurs occupés
    int i;            // compte-tours de boucle
    int liv;          // un livreur disponible
    int nval;         // nombre d'éléments lus par scanf
    char nom[MAXNOM]; // nom du client
    int duree;        // durée de la course en ms

    pthread_mutex_t mutex_emetteur;
    pthread_cond_t *cond_emetteur;
    pthread_cond_t cond_livreur;
    pthread_mutex_t mutex_livreur;
    pthread_cond_init(&cond_livreur, NULL);
    pthread_mutex_init(&mutex_livreur, NULL);
    pthread_mutex_init(&mutex_emetteur, NULL);

    if (argc != 2 || (nthreads = atoi(argv[1])) <= 0)
        raler(0, "usage: %s nb-threads\n\tavec nb-threads > 0", argv[0]);

    cond_emetteur = malloc(sizeof(pthread_cond_t) * nthreads);
    for (int i = 0; i < nthreads; i++)
        pthread_cond_init(&cond_emetteur[i], NULL);

    // calloc initialise toute la mémoire à 0
    NCHK(tid = calloc(nthreads + 1, sizeof *tid));
    NCHK(occupes = calloc(nthreads, sizeof *occupes));
    NCHK(targ = calloc(nthreads + 1, sizeof *targ));

    // Créer les threads livreurs
    for (i = 0; i < nthreads; i++) {
        targ[i].nom[0] = '\0'; // pas de course pour le moment
        targ[i].duree = 0;     // pas vraiment nécessaire
        targ[i].occupes = occupes;
        // TODO à compléter

        targ[i].nb_livreurs = i;
        targ[i].cond_livreur = &cond_livreur;
        targ[i].cond_emetteur = &cond_emetteur[i];
        targ[i].mutex_livreur = &mutex_livreur;
        targ[i].mutex_emetteur = &mutex_emetteur;

        TCHK(pthread_create(&tid[i], NULL, livreur, &targ[i]));
    }

    // lire les courses à effectuer et les affecter aux livreurs
    while ((nval = scanf("%s%d\n", nom, &duree)) == 2) {
        // Tests
        if (strlen(nom) >= MAXNOM)
            raler(0, "nom '%s' trop long pour %d caractères", nom, MAXNOM);
        if (duree < 0)
            raler(0, "duree '%d' devrait être >= 0", duree);

        // TODO attendre qu'au moins un livreur soit prêt
        while ((liv = trouver_livreur_pret(occupes, nthreads)) == -1)
            ; // attente active... beuark ! => il faut une
              // attente passive

        printf("Course %s (duree = %d) -> livreur %d\n", nom, duree, liv);
        occupes[liv] = 1; // occupé => ne pas réutiliser tout de suite

        // TODO donner la course au livreur trouvé et le réveiller
        TCHK(pthread_mutex_lock(&mutex_livreur));
        strcpy(targ[liv].nom, nom);
        targ[liv].duree = duree;
        TCHK(pthread_mutex_unlock(&mutex_livreur));
        TCHK(pthread_cond_signal(&cond_livreur));
    }

    if (nval != EOF)
        raler(0, "mauvais format pour <nom, duree>");

    // prévenir tous les livreurs qu'ils doivent s'arrêter
    for (i = 0; i < nthreads; i++) {
        // TODO le livreur i est-il occupé ? si oui, attendre
        TCHK(pthread_mutex_lock(&mutex_emetteur));
        if (occupes[i] == 1) {
            TCHK(pthread_cond_wait(&cond_emetteur[i], &mutex_emetteur));
        }
        TCHK(pthread_mutex_unlock(&mutex_emetteur));

        // TODO prévenir le livreur i qu'il doit s'arrêter
        TCHK(pthread_mutex_lock(&mutex_livreur));
        strcpy(targ[i].nom, "bidon");
        targ[i].duree = -1;
        TCHK(pthread_mutex_unlock(&mutex_livreur));
        TCHK(pthread_cond_signal(&cond_livreur));

        // Attendre la terminaison du livreur i
        TCHK(pthread_join(tid[i], NULL));
    }

    free(tid);
    free(targ);
    free(occupes);

    TCHK(pthread_mutex_destroy(&mutex_livreur));
    TCHK(pthread_cond_destroy(&cond_livreur));
    TCHK(pthread_mutex_destroy(&mutex_emetteur));
    for (int i = 0; i < nthreads; i++)
        pthread_cond_destroy(&cond_emetteur[i]);

    free(cond_emetteur);

    exit(0);
}
