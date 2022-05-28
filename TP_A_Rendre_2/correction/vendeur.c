#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdnoreturn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "lib.h"

int main (int argc, const char *argv [])
{
    struct stat stbuf ;
    const char *produit ;
    int i, quantite ;
    struct acces a ;
    struct etal e ;
    int creer ;

    if (argc != 3)
	raler (0, "usage: %s produit quantite", argv [0]) ;

    produit = argv [1] ;
    quantite = atoi (argv [2]) ;

    if (quantite < 0)
	raler (0, "la quantité ne peut être négative") ;

    creer = 0 ;
    if (stat (produit, &stbuf) == -1)
    {
	if (errno != ENOENT)		// Erreur non reconnue
	    raler (1, "cannot access %s", produit) ;
	if (quantite == 0)
	    raler (0, "Je ne peux pas fermer un étal inexistant") ;
	creer = 1 ;
    }
    a = open_etal (produit, creer) ;
    // l'étal est verrouillé

    // ajouter les produits à l'étal
    lire_etal (&a, &e) ;
    if (quantite > 0)
	e.nprod += quantite ;
    else
    {
	e.nprod = -1 ;			// fermer l'étal
	supprimer_etal (produit) ;	// empêcher de nouveaux clients de venir
    }

    // réveiller tous les clients en attente
    for (i = 0 ; i < e.natt ; i++)
	V (a.tabsem [SEM_ATT]) ;

    e.natt = 0 ;

    ecrire_etal (&a, &e) ;
    close_etal (&a) ;

    exit (0) ;
}
