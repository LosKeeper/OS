#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <semaphore.h>
#include <fcntl.h>

#include "lib.h"


noreturn void raler (int syserr, const char *fmt, ...)
{
    va_list ap ;

    va_start (ap, fmt) ;
    vfprintf (stderr, fmt, ap) ;
    fprintf (stderr, "\n") ;
    va_end (ap) ;
    if (syserr)
	perror ("") ;
    exit (1) ;
}

// Retourne un tableau contenant les 2 noms de sémaphores
char **nom_semaphores (const char *produit)
{
    static char noms [2][NOM_SEMAPHORE] ;
    static char *tab [] = { noms [0], noms [1] } ;

    // vérification de la longueur du nom du produit
    if (strlen (produit) >= NOM_PRODUIT)
	raler (0, "'%s' est un nom de produit trop long", produit) ;

    // pas de vérification de taille, car NOM_SEMAPHORE est correctement défini
    (void) sprintf (tab [SEM_XCL], "/%s.xcl", produit) ;
    (void) sprintf (tab [SEM_ATT], "/%s.att", produit) ;

    return tab ;
}

// Plus facile à lire que sem_wait
void P (sem_t *sem)
{
    CHK (sem_wait (sem)) ;
}

// Plus facile à lire que sem_post
void V (sem_t *sem)
{
    CHK (sem_post (sem)) ;
}

// Lit le contenu de l'étal. On suppose que le fichier est verrouillé.
void lire_etal (struct acces *a, struct etal *e)
{
    CHK (lseek (a->fd, 0, SEEK_SET)) ;
    CHK (read (a->fd, e, sizeof *e)) ;
}

// Écrit le contenu de l'étal. On suppose que le fichier est verrouillé
void ecrire_etal (struct acces *a, struct etal *e)
{
    CHK (lseek (a->fd, 0, SEEK_SET)) ;
    CHK (write (a->fd, e, sizeof *e)) ;
}

// Retourne les descripteurs donnant un accès (exclusif) à l'étal
struct acces open_etal (const char *produit, int creer)
{
    struct acces a ;
    int modef, modes ;		// modes d'ouverture du fichier et des sem
    char **tabnom ;
    struct etal e = {.nprod = 0, .natt = 0} ;

    tabnom = nom_semaphores (produit) ;

    modef = O_RDWR ;
    modes = 0 ;
    if (creer)
    {
	modef |= O_CREAT | O_TRUNC ;
	modes |= O_CREAT ;
	// supprimer les sémaphores au cas où ils existeraient encore
	(void) unlink (tabnom [SEM_XCL]) ;
	(void) unlink (tabnom [SEM_ATT]) ;
    }

    // on donne les 2 derniers arguments au cas où on crée (modes = O_CREAT)
    SCHK (a.tabsem [SEM_XCL] = sem_open (tabnom [SEM_XCL], modes, 0666, 1)) ;
    SCHK (a.tabsem [SEM_ATT] = sem_open (tabnom [SEM_ATT], modes, 0666, 0)) ;

    // demande d'accès exclusif au fichier
    P (a.tabsem [SEM_XCL]) ;
    CHK (a.fd = open (produit, modef, 0666)) ;

    if (creer)
	ecrire_etal (&a, &e) ;

    return a ;
}

// Ferme l'accès à l'étal, avec déverrouillage préalable
void close_etal (struct acces *a)
{
    // libère l'accès à l'étal
    V (a->tabsem [SEM_XCL]) ;

    CHK (sem_close (a->tabsem [SEM_XCL])) ;
    CHK (sem_close (a->tabsem [SEM_ATT])) ;
    CHK (close (a->fd)) ;
}

// Cette fonction n'est pas commune, mais on la met ici pour
// concentrer tous les accès à nom_semaphore()
void supprimer_etal (const char *produit)
{
    char **tabnom ;

    CHK (unlink (produit)) ;

    tabnom = nom_semaphores (produit) ;
    CHK (sem_unlink (tabnom [SEM_XCL])) ;
    CHK (sem_unlink (tabnom [SEM_ATT])) ;
}
