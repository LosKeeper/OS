#include "ex2.h"

int main(int argc, char *argv[]) {
    if (argc != 2)
        raler(1, "Usage: %s <nrepas>", argv[0]);

    int nrepas = atoi(argv[1]);

    int fd;
    CHK(fd = shm_open("/table", O_RDWR, 0666));

    struct table_s *table;
    table = mmap(NULL, sizeof(struct table_s), PROT_READ | PROT_WRITE,
                 MAP_SHARED, fd, 0);
    if (table == MAP_FAILED)
        raler(1, "mmap");

    table =
        mmap(NULL, sizeof(struct table_s) + table->nb_couverts * sizeof(sem_t),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (table == MAP_FAILED)
        raler(1, "mmap");

    CHK(close(fd));

    // Read struct from memory
    int nb_couverts = table->nb_couverts;
    int id = table->nb_next;

    for (int i = 0; i < nrepas; i++) {
        TCHK(sem_wait(&table->sem[i]));
        TCHK(sem_wait(&table->sem[(i + 1) % nb_couverts]));
        printf("%d is eating !\n", id);
        fflush(stdout);
        TCHK(sem_post(&table->sem[i]));
        TCHK(sem_post(&table->sem[(i + 1) % nb_couverts]));
    }

    TCHK(sem_wait(&table->done));
}