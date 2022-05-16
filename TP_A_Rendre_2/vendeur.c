// Fichier vendeur.c à rédiger

#include "lib.h"

int main(int argc, char *argv[]) {
    // Check arguments
    if (argc != 3) {
        raler(0, "Usage: %s <produit> <quantite>\n", argv[0]);
    }

    // Get arguments
    char *produit = argv[1];
    int quantite = atoi(argv[2]);

    // Create named semaphore for the product oe use existing one
    sem_t *sem;
    sem = sem_open(produit, O_CREAT | O_RDWR, 0666, 1);
    if (sem == SEM_FAILED) {
        raler(1, "sem_open");
    }

    // Wait for the product to be available
    sem_wait(sem);

    // Create file named "produit" in the current directory or truncate it
    int fd;
    CHK(fd = open(produit, O_WRONLY | O_CREAT | O_TRUNC, 0666));

    // Write "quantite" to the file if file is empty
    if (lseek(fd, 0, SEEK_END) == 0) {
        CHK(write(fd, &quantite, sizeof(int)));
    } else {
        // Read the file
        int q;
        CHK(read(fd, &q, sizeof(q)));

        // Update the file
        q += quantite;
        CHK(lseek(fd, 0, SEEK_SET));
        CHK(write(fd, &q, sizeof(q)));
    }

    // Product is now available
    sem_post(sem);

    // Close file and semaphore
    CHK(close(fd));
    CHK(sem_close(sem));

    // Exit
    exit(EXIT_SUCCESS);
}
