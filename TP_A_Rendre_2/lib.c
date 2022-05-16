#include "lib.h"

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

void *chk_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL)
        raler(0, "malloc");
    return ptr;
}

char *sem_name(char *name, short type) {
    char *sem_name = chk_malloc(strlen(name) + SEM_NAME_EXT_LEN);
    switch (type) {
    case 0:
        sprintf(sem_name, "%s.file", name);
        break;
    case 1:
        sprintf(sem_name, "%s.vendeur", name);
        break;
    case 2:
        sprintf(sem_name, "%s.client", name);
        break;
    default:
        raler(0, "sem_name: type %d unknow", type);
    }
    return sem_name;
}

sem_t *chk_sem_open(char *name, short type, int value) {
    sem_t *sem = sem_open(sem_name(name, type), O_CREAT | O_RDWR, 0666, value);
    if (sem == SEM_FAILED)
        raler(0, "sem_open");
    return sem;
}
