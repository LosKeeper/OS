#include "lib.h"

int main(int argc, char *argv[]) {
    // Check arguments
    if (argc != 3) {
        raler(0, "Usage: %s <produit> <quantite>", argv[0]);
    }

    // Get arguments
    char *produit = argv[1];
    int quantite = atoi(argv[2]);
    // Check if the quantity is a positive integer
    if (quantite < 0) {
        raler(0, "Usage: %s <produit> <quantite>", argv[0]);
    }
    struct product_file_s product_file;
    product_file.quantite = quantite;

    // Create named semaphore for the file or use existing one
    sem_t *sem_file;
    sem_file = chk_sem_open(produit, 0, 1);

    // Create named semaphore for the "vendeur" or use existing one
    sem_t *sem_vendeur;
    sem_vendeur = chk_sem_open(produit, 1, 1);

    // Create named semaphore for the "client" or use existing one
    sem_t *sem_client;
    sem_client = chk_sem_open(produit, 2, 0);

    // Wait for the file to be available
    TCHK(sem_wait(sem_file));

    // Wait for the "vendeur" to be warn by the "client"
    TCHK(sem_wait(sem_vendeur));

    // Create file named "produit" in the current directory or truncate it
    int fd;
    CHK(fd = open(produit, O_WRONLY | O_CREAT | O_TRUNC, 0666));

    // Check if the "vendeur" want to close
    if (quantite == 0) {
        // The "vendeur" want to close

        // Check if the product file exists
        if (lseek(fd, 0, SEEK_END) <= 0) {
            // The product file is empty
            // Remove file
            CHK(close(fd));
            CHK(unlink(produit));
            raler(0, "Product file %s is empty\n", produit);
        } else {
            // Move the file pointer to the beginning of the file
            CHK(lseek(fd, 0, SEEK_SET));
        }

        // Warn the "client" that the "vendeur" want to close
        TCHK(sem_post(sem_client));

        // Wait for the "client" acknowledge
        TCHK(sem_wait(sem_vendeur));

        // Close the file and semaphores
        TCHK(close(fd));
        TCHK(sem_close(sem_file));
        TCHK(sem_close(sem_vendeur));
        TCHK(sem_close(sem_client));

        // Exit
        exit(EXIT_SUCCESS);
    }

    // Move the file pointer to the beginning of the file
    CHK(lseek(fd, 0, SEEK_SET));

    // Write "quantite" to the file if file is empty
    if (lseek(fd, 0, SEEK_END) <= 0) {
        CHK(write(fd, &product_file, sizeof(product_file)));
    } else {
        // Move the file pointer to the beginning of the file
        CHK(lseek(fd, 0, SEEK_SET));

        // Read the file
        struct product_file_s product_file_read;
        CHK(read(fd, &product_file_read, sizeof(product_file_read)));

        // Update the file
        product_file_read.quantite += quantite;
        CHK(lseek(fd, 0, SEEK_SET));
        CHK(write(fd, &product_file_read, sizeof(product_file_read)));
    }

    // The file is now available
    TCHK(sem_post(sem_file));

    // Warn the "client" that the file is available
    TCHK(sem_post(sem_client));

    // Close file and semaphores
    CHK(close(fd));
    CHK(sem_close(sem_file));
    CHK(sem_close(sem_vendeur));
    CHK(sem_close(sem_client));

    // Exit
    exit(EXIT_SUCCESS);
}
