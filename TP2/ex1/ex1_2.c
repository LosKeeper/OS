#include "../../macro.h"

long int compteur;

void *f(void *arg) {
    pthread_mutex_t *mutex = (pthread_mutex_t *)arg;
    for (int k = 0; k < 1000 * 1000; k++) {
        TCHK(pthread_mutex_lock(mutex));
        compteur++;
        TCHK(pthread_mutex_unlock(mutex));
    }
    return NULL;
}

int main() {
    pthread_t t[4];

    pthread_mutex_t m;
    TCHK(pthread_mutex_init(&m, NULL));

    for (int i = 0; i < 4; i++) {
        TCHK(pthread_create(&t[i], NULL, f, &m));
    }

    for (int i = 0; i < 4; i++) {
        TCHK(pthread_join(t[i], NULL));
    }

    TCHK(pthread_mutex_destroy(&m));

    printf("compteur = %ld\n", compteur);
    return 0;
}