#include "../macro.h"

unsigned int nbMachines;

void *job(void *arg) {
    unsigned int *t = arg;
    sleep(*t);
    return NULL;
}

struct thread_arg {
    unsigned int p;
    unsigned int m;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    void *(*function)(void *);
};

void *thread_function(void *arg) {
    // Get arguments
    struct thread_arg *targ = arg;
    unsigned int p = targ->p;
    unsigned int m = targ->m;
    pthread_cond_t *cond = targ->cond;
    pthread_mutex_t *mutex = targ->mutex;
    void *(*function)(void *) = targ->function;

    // Get number of machines for each jobs
    unsigned int *k = malloc(sizeof(unsigned int) * p);
    if (k == NULL) {
        raler(0, "malloc");
    }
    k[0] = 1 + ((unsigned int)time(NULL) % m);
    for (unsigned int i = 1; i < p; i++) {
        k[i] = 1 + (rand_r(&k[i - 1]) % m);
    }

    // Get time for each sleep
    unsigned int *t = malloc(sizeof(unsigned int) * p);
    if (t == NULL) {
        raler(0, "malloc");
    }
    t[0] = 1 + ((unsigned int)time(NULL) % 3);
    for (unsigned int i = 0; i < p; i++) {
        t[i] = rand_r(&t[i - 1]) % 3;
    }

    // Launch jobs
    for (unsigned int j = 1; j <= p; j++) {
        // Wait for k[j] machines available
        TCHK(pthread_mutex_lock(mutex));
        while (nbMachines < k[j - 1]) {
            printf("Thread %d: waiting for %d machines\n", j, k[j - 1]);
            fflush(stdout);
            TCHK(pthread_cond_wait(cond, mutex));
        }
        nbMachines -= k[j - 1];
        TCHK(pthread_mutex_unlock(mutex));
        function(&t[j - 1]);
        TCHK(pthread_mutex_lock(mutex));
        nbMachines += k[j - 1];
        TCHK(pthread_mutex_unlock(mutex));
        TCHK(pthread_cond_broadcast(cond));
    }

    free(k);
    return NULL;
}

int main(int argc, char **argv) {
    // Check arguments
    if (argc != 4) {
        raler(0, "Usage: %s <m> <n> <p>", argv[0]);
    }

    // Get arguments
    unsigned int m = atoi(argv[1]);
    nbMachines = m;
    unsigned int n = atoi(argv[2]);
    unsigned int p = atoi(argv[3]);

    // Initialize threads
    pthread_t *threads = malloc(sizeof(pthread_t) * n);

    // Initialize conditions
    pthread_cond_t cond;
    TCHK(pthread_cond_init(&cond, NULL));
    pthread_mutex_t mutex;
    TCHK(pthread_mutex_init(&mutex, NULL));

    // Initialize arguments
    struct thread_arg arg = {p, m, &cond, &mutex, job};

    // Launch threads
    for (unsigned int i = 0; i < n; i++) {
        TCHK(pthread_create(threads + i, NULL, thread_function, &arg));
    }

    // Wait for threads
    for (unsigned int i = 0; i < n; i++) {
        TCHK(pthread_join(threads[i], NULL));
    }

    free(threads);
    TCHK(pthread_cond_destroy(&cond));
    TCHK(pthread_mutex_destroy(&mutex));
    exit(EXIT_SUCCESS);
}