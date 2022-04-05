#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#define CHK(op)                                                                \
    do {                                                                       \
        if ((op) == -1)                                                        \
            raler(1, #op);                                                     \
    } while (0)

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

struct thread_args_t {
    int p;
    int t;
};

void *fonction(void *arg) {
    int *u = malloc(sizeof(int));
    *u = 0;
    struct thread_args_t *args = (struct thread_args_t *)arg;
    int p = args->p;
    int t = args->t;
    int i;
    for (i = 1; i <= p; i++) {
        *u += (t - 1) * p + i;
    }
    return u;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <n> <p>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int p = atoi(argv[2]);
    int n = atoi(argv[1]);
    pthread_t *tid = calloc(n, sizeof(pthread_t));
    struct thread_args_t *args = calloc(n, sizeof(struct thread_args_t));
    int t;
    for (t = 1; t <= n; t++) {
        args[t - 1].p = p;
        args[t - 1].t = t;
        if ((errno = (pthread_create(&tid[t - 1], NULL, fonction,
                                     &args[t - 1]))) > 0)
            raler(1, "pthread_create");
    }
    int *u;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        if ((errno = (pthread_join(tid[i], (void **)&u))) > 0)
            raler(1, "pthread_join");
        sum += *u;
        free(u);
    }
    printf("%d\n", sum);
    free(tid);
    free(args);
    exit(0);
}