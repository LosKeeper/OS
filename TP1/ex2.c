#include "macro.h"

struct thread_args_t {
    int p;
    int t;
    pthread_barrier_t *bar;
    char *chaine;
};

void *fonction(void *arg) {
    struct thread_args_t *args = (struct thread_args_t *)arg;
    int p = args->p;
    int t = args->t;
    int i;
    for (i = 0; i < p; i++) {
        if ((errno = pthread_barrier_wait(args->bar)) > 0)
            raler(1, "pthread_barrier_wait");
        args->chaine[t] = i % 2 ? '-' : '#';
        if ((errno = pthread_barrier_wait(args->bar)) > 0) {
            raler(1, "pthread_barrier_wait");
        } else if (errno == PTHREAD_BARRIER_SERIAL_THREAD) {
            printf("%s\n", (args->chaine));
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <n> <p>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int p = atoi(argv[2]);
    pthread_barrier_t bar;
    char *chaine = malloc(sizeof(char) * (n + 1));
    chaine[n] = '\0';
    CHK(pthread_barrier_init(&bar, NULL, n));
    struct thread_args_t *args = calloc(n, sizeof(struct thread_args_t));
    pthread_t *tid = calloc(n, sizeof(pthread_t));
    int t;
    for (t = 0; t < n; t++) {
        args[t].p = p;
        args[t].t = t;
        args[t].bar = &bar;
        args[t].chaine = chaine;
        if ((errno = (pthread_create(&tid[t], NULL, fonction, &args[t]))) > 0)
            raler(1, "pthread_create");
    }
    for (int i = 0; i < n; i++) {
        if ((errno = (pthread_join(tid[i], NULL))) > 0)
            raler(1, "pthread_join");
    }
    CHK(pthread_barrier_destroy(&bar));
    free(chaine);
    free(args);
    free(tid);
    exit(EXIT_SUCCESS);
}