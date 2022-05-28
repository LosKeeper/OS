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

struct courses
{
    const char *produit ;
    int quantite ;
} ;

int main (int argc, const char *argv [])
{
    int i, ncourses ;
    struct acces a ;
    struct etal e ;
    struct courses *c ;			// la liste de courses

    if (argc % 2 == 0)
	raler (0, "usage: %s prod1 quant1 ... prodn quantn", argv [0]) ;

    ncourses = (argc - 1) / 2 ;
    c = calloc (ncourses, sizeof *c) ;
    if (c == NULL)
	raler (1, "cannot calloc liste de %d courses", ncourses) ;

    // Vérification des arguments. On stocke tout dans un tableau
    // pour éviter de refaire des actions en double (strlen + atoi)
    // et ainsi sauver la planète
    for (i = 0 ; i < ncourses ; i++)
    {
	c [i].produit = argv [2*i + 1] ;
	c [i].quantite = atoi (argv [2*i+2]) ;

	if (strlen (c [i].produit) >= NOM_PRODUIT)
	    raler (0, "'%s' est un nom de produit trop long", c [i].produit) ;

	if (c [i].quantite <= 0)
	    raler (0, "Je ne peux pas acheter %d %s",
			    c[i].quantite, c [i].produit) ;
    }

    // Acheter les produits demandés
    for (i = 0 ; i < ncourses ; i++)
    {
	// accès exclusif à l'étal
	a = open_etal (c [i].produit, 0) ;

	for (;;)
	{
	    // On a le verrou pour accéder au fichier
	    lire_etal (&a, &e) ;
	    if (e.nprod == -1)
	    {
		// Le vendeur est parti.
		// Ne pas oublier de déverrouiller le fichier
		V (a.tabsem [SEM_XCL]) ;
		raler (0, "Le vendeur de %s ferme", c [i].produit) ;
	    }

	    if (c [i].quantite <= e.nprod)
		break ;			// on peut acheter sans attendre

	    // Le produit n'est pas disponible en quantité suffisante.
	    // Il faut attendre que le commerçant réapprovisionne son étal
	    e.natt++ ;			// on se met en attente
	    ecrire_etal (&a, &e) ;
	    V (a.tabsem [SEM_XCL]) ;	// libérer le fichier

	    // attente
	    P (a.tabsem [SEM_ATT]) ;

	    // reprendre le verrou pour accéder au fichier
	    P (a.tabsem [SEM_XCL]) ;
	}

	// ici, le fichier est verrouillé, et e est l'étal contenant
	// la quantité souhaitée
	e.nprod -= c [i].quantite ;
	ecrire_etal (&a, &e) ;
	printf ("J'ai acheté %d %s\n", c [i].quantite, c [i].produit) ;

	// Fermer et libérer l'accès exclusif à l'étal
	close_etal (&a) ;
    }

    free (c) ;

    exit (0) ;
}
