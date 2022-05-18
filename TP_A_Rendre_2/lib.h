#pragma once

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#define SEM_NAME_EXT_LEN \
    9 // "sem.PROD.file" or "sem.PROD.vendeur" or "sem.PROD.client"

#define TCHK(op)                \
    do {                        \
        if ((errno = (op)) > 0) \
            raler(1, #op);      \
    } while (0)

#define CHK(op)            \
    do {                   \
        if ((op) == -1)    \
            raler(1, #op); \
    } while (0)

#define DEBUG 1

// Print debug in green
#if DEBUG
#define DEBUG_PRINT(...)              \
    do {                              \
        fprintf(stderr, GREEN);       \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, RESET);       \
    } while (0)
#else
#define DEBUG_PRINT(...)
#endif

/**
 * @brief Structure for the arguments of the client
 */
struct client_args {
    char *produit;
    int quantite;
};

/**
 * @brief Structure for the content of the product file
 */
struct product_file_s {
    int quantite;
    int nb_clients;
};

/**
 * @brief   Fubction that print an error message and exit the program in case of
 * error.
 * @param   syserr  1 errno is set, 0 otherwise
 * @param   msg     The message
 */
noreturn void raler(int syserr, const char *msg, ...);

/**
 * @brief   Function that allocate memory and exit the program in case of error.
 * @param   size    The size of the memory to allocate
 * @return  The allocated memory
 */
void *chk_malloc(size_t size);

/**
 * @brief  Function that return the name of the semaphore to use.
 * @param  name     The name of the product
 * @param  type     0 if it concerns the semaphore for the file, 1 if it
 * concerns the semaphore for the "vendeur", 2 if it concerns the semaphore for
 * the "client"
 * @return The name of the semaphore to use
 */
char *sem_name(char *name, short type);

/**
 * @brief   Function that open a semaphore and exit the program in case of
 * error.
 * @param   name    The name of the product
 * @param   type    0 if it concerns the semaphore for the file, 1 if it
 * concerns the semaphore for the "vendeur", 2 if it concerns the semaphore for
 * the "client"
 * @param   value   The start value of the semaphore
 * @return  The semaphore
 */
sem_t *chk_sem_open(char *name, short type, int value);
