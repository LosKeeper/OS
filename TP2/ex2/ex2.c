#include "../../macro.h"
#include <semaphore.h>
#include <unistd.h>
#define NBTHMAX 9
#define NBSEM 6

#define _21 0
#define _22 1
#define _23 2
#define _24 3
#define _31 4
#define _32 5

int task_number(int n) {
    switch (n) {
    case 0:
        return 11;

    case 1:
        return 12;

    case 2:
        return 13;

    case 3:
        return 21;

    case 4:
        return 22;

    case 5:
        return 23;

    case 6:
        return 24;

    case 7:
        return 31;

    case 8:
        return 32;
    }
    return -1;
}

sem_t sem[NBSEM];

struct thread_arg {
    int id;
};

void task(int n) {
    printf("task %d start\n", n);
    fflush(stdout);

    sleep(1); // simulate some work

    printf("task %d end\n", n);
    fflush(stdout);
}

void *f(void *arg) {
    struct thread_arg *data = (struct thread_arg *)arg;

    switch (data->id) {
    case 11:
        task(data->id);
        CHK(sem_post(&sem[_21]));
        CHK(sem_post(&sem[_22]));
        break;

    case 12:
        task(data->id);
        CHK(sem_post(&sem[_21]));
        CHK(sem_post(&sem[_22]));
        CHK(sem_post(&sem[_23]));
        break;

    case 13:
        task(data->id);
        CHK(sem_post(&sem[_23]));
        CHK(sem_post(&sem[_24]));
        CHK(sem_post(&sem[_31]));
        break;

    case 21:
        CHK(sem_wait(&sem[_21]));
        CHK(sem_wait(&sem[_21]));
        task(data->id);
        CHK(sem_post(&sem[_31]));
        break;

    case 22:
        CHK(sem_wait(&sem[_22]));
        CHK(sem_wait(&sem[_22]));
        task(data->id);
        CHK(sem_post(&sem[_32]));
        break;

    case 23:
        CHK(sem_wait(&sem[_23]));
        CHK(sem_wait(&sem[_23]));
        task(data->id);
        CHK(sem_post(&sem[_32]));
        break;

    case 24:
        CHK(sem_wait(&sem[_24]));
        task(data->id);
        CHK(sem_post(&sem[_32]));
        break;

    case 31:
        CHK(sem_wait(&sem[_31]));
        CHK(sem_wait(&sem[_31]));
        task(data->id);
        break;

    case 32:
        CHK(sem_wait(&sem[_32]));
        CHK(sem_wait(&sem[_32]));
        CHK(sem_wait(&sem[_32]));
        task(data->id);
        break;
    }
    return NULL;
}

int main(void) {
    pthread_t t[NBTHMAX];

    for (int i = 0; i < NBSEM; i++) {
        CHK(sem_init(&sem[i], 0, 0));
    }

    struct thread_arg data[NBTHMAX];
    for (int i = 0; i < NBTHMAX; i++) {
        data[i].id = task_number(i);
        TCHK(pthread_create(&t[i], NULL, f, &data[i]));
    }

    for (int i = 0; i < NBSEM; i++) {
        CHK(sem_destroy(&sem[i]));
    }

    for (int i = 0; i < NBTHMAX; i++) {
        TCHK(pthread_join(t[i], NULL));
    }
}