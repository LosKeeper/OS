#include "ex2.h"

int main(int argc, char *argv[]) {
    if (argc != 2)
        raler(1, "Usage: %s <nphilo>", argv[0]);

    int nphilo = atoi(argv[1]);

    int fd;
    CHK(fd = shm_open("/table", O_CREAT | O_TRUNC | O_RDWR, 0666));

    struct table_s *table;
    CHK(ftruncate(fd, sizeof(struct table_s) + nphilo * sizeof(sem_t)));
    table = mmap(NULL, sizeof(struct table_s) + nphilo * sizeof(sem_t),
                 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (table == MAP_FAILED)
        raler(1, "mmap");

    CHK(close(fd));

    // Init struct in memory
    table->nb_couverts = nphilo;
    table->nb_next = 0;

    // Init semaphores
    TCHK(sem_init(&table->done, 1, -nphilo));
    for (int i = 0; i < nphilo; i++) {
        TCHK(sem_init(&table->sem[i], 1, 1));
    }

    TCHK(sem_wait(&table->done));
    exit(EXIT_SUCCESS);
}