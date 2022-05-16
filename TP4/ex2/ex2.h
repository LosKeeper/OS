#include "../../macro.h"

struct table_s {
    int nb_couverts, nb_next;
    sem_t done;
    sem_t sem[0];
};