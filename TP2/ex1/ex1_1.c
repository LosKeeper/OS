#include ";;/../macro.h"

long int compteur;

void *f() {
    for (int k = 0; k < 1000 * 1000; k++) {
        compteur++;
    }
    return NULL;
}

int main() {
    pthread_t t[4];

    for (int i = 0; i < 4; i++) {
        TCHK(pthread_create(&t[i], NULL, f, NULL));
    }

    for (int i = 0; i < 4; i++) {
        TCHK(pthread_join(t[i], NULL));
    }

    printf("compteur = %ld\n", compteur);
    return 0;
}