#include "lib.h"

int main(int argc, char *argv[]) {
    // Check arguments
    if (argc % 2 != 1 || argc < 3) {
        raler(0,
              "Usage: %s <produit_1> <quantite_1> <produit_2> <quantite_2> ...",
              argv[0]);
    }

    // Get arguments
    struct client_args *arguments =
        chk_malloc(sizeof(struct client_args) * ((argc - 1) / 2));
    for (int i = 0; i < (argc - 1) / 2; i++) {
        arguments[i].produit = argv[2 * i + 1];
        arguments[i].quantite = atoi(argv[2 * i + 2]);
        if (arguments[i].quantite <= 0) {
            raler(0,
                  "Usage: %s <produit_1> <quantite_1> <produit_2> <quantite_2> "
                  "...",
                  argv[0]);
        }
    }

    // Create named semaphore for the file or use existing one
    sem_t *sem_file;
    sem_file = chk_sem_open(arguments[0].produit, 0, 1);

    // Create named semaphore for the "vendeur" or use existing one
    sem_t *sem_vendeur;
    sem_vendeur = chk_sem_open(arguments[0].produit, 1, 1);

    // Create named semaphore for the "client" or use existing one
    sem_t *sem_client;
    sem_client = chk_sem_open(arguments[0].produit, 2, 0);

    // Wait for the file to be available
    TCHK(sem_wait(sem_file));

    // Wait for the "vendeur" to warn that the file is available
    TCHK(sem_wait(sem_client));

    // Read the product file
    int fd;
    CHK(fd = open(arguments[0].produit, O_RDWR));

    // Check if the file is empty
    if (lseek(fd, 0, SEEK_END) == 0) {
        // The file is empty
        raler(0, "Product file %s is empty\n", arguments[0].produit);
    } else {
        // Move the file pointer to the beginning of the file
        CHK(lseek(fd, 0, SEEK_SET));
    }

    // Read the file
    struct product_file_s product_file_read;
    CHK(read(fd, &product_file_read, sizeof(product_file_read)));
    printf("%s: %d\n", arguments[0].produit, product_file_read.quantite);
    product_file_read.quantite -= arguments[0].quantite;
    printf("%s: %d\n", arguments[0].produit, product_file_read.quantite);
    // Update the file
    CHK(lseek(fd, 0, SEEK_SET));
    CHK(write(fd, &product_file_read, sizeof(product_file_read)));

    // The file is now available
    TCHK(sem_post(sem_file));

    // Warn the "vendeur" that the file is available
    TCHK(sem_post(sem_vendeur));

    // Close file and semaphores
    CHK(close(fd));
    CHK(sem_close(sem_file));
    CHK(sem_close(sem_vendeur));
    CHK(sem_close(sem_client));

    // Exit
    exit(EXIT_SUCCESS);
}
