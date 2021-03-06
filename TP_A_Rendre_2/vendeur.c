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
    product_file.nb_clients = 0;

    // Create named semaphore for the file or use existing one
    sem_t *sem_file;
    sem_file = chk_sem_open(produit, 0, 1);

    // Create named semaphore for the "vendeur" or use existing one
    sem_t *sem_vendeur;
    sem_vendeur = chk_sem_open(produit, 1, 0);

    // Wait for the file to be available
    TCHK(sem_wait(sem_file));

    // Create file named "produit" in the current directory or open it
    int fd;
    CHK(fd = open(produit, O_RDWR | O_CREAT, 0666));

    // Check if the "vendeur" want to close
    if (quantite == 0) {
        // The "vendeur" want to close

        // Check if the product file exists
        if (lseek(fd, 0, SEEK_END) <= 0) {
            // The product file is empty
            // Remove file
            CHK(close(fd));
            CHK(sem_close(sem_file));
            CHK(unlink(produit));
            CHK(sem_unlink(sem_name(produit, 0)));
            raler(0, "Product file %s is empty\n", produit);
        }

        // Move the file pointer to the beginning of the file
        CHK(lseek(fd, 0, SEEK_SET));

        // Read the file
        CHK(read(fd, &product_file, sizeof(product_file)));

        // Close the file and semaphores
        CHK(close(fd));
        CHK(sem_close(sem_file));
        CHK(unlink(produit));
        CHK(sem_unlink(sem_name(produit, 0)));
        DEBUG_PRINT("Product file %s closed\n", produit);

        // Warn the "client" that the "vendeur" is closed
        for (int i = 0; i < product_file.nb_clients; i++) {
            TCHK(sem_post(sem_vendeur));
        }
        DEBUG_PRINT("%d clients notified\n", product_file.nb_clients);

        exit(EXIT_SUCCESS);
    }

    // Write quantity to the file if file is empty
    int n;
    if ((n = lseek(fd, 0, SEEK_END)) == 0) {
        // Move the file pointer to the beginning of the file
        CHK(lseek(fd, 0, SEEK_SET));
        CHK(write(fd, &product_file, sizeof(product_file)));
    } else if (n < 0) {
        // Error
        raler(1, "lseek");
    } else {
        // Move the file pointer to the beginning of the file
        CHK(lseek(fd, 0, SEEK_SET));

        // Read the file
        CHK(read(fd, &product_file, sizeof(product_file)));

        // Update the file
        product_file.quantite += quantite;
        CHK(lseek(fd, 0, SEEK_SET));
        CHK(write(fd, &product_file, sizeof(product_file)));
        DEBUG_PRINT("%d are currently waiting for the %s\n",
                    product_file.nb_clients, produit);
    }

    // Warn every waiting "client" that the file is available
    for (int i = 0; i < product_file.nb_clients; i++) {
        TCHK(sem_post(sem_vendeur));
    }
    DEBUG_PRINT("%d clients notified\n", product_file.nb_clients);

    // The file is now available
    TCHK(sem_post(sem_file));

    // Close file and semaphores
    CHK(close(fd));
    CHK(sem_close(sem_file));
    CHK(sem_close(sem_vendeur));

    exit(EXIT_SUCCESS);
}
