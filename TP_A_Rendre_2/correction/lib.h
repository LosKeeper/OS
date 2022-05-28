/*
 * Ce fichier comprend les définitions communes aux deux programmes
 *
 * Schéma de synchronisation : parmi tous les schémas de synchronisation
 * possibles, on peut en distinguer 2, qui utilisent peu ou prou les
 * mêmes sémaphores :
 * - sémaphore d'exclusion mutuelle (donc initialisé à 1) pour
 *   protéger les accès au fichier
 * - sémaphore d'attente (donc initialisé à 0) pour permettre aux
 *   clients d'attendre le réassort par le vendeur
 * Ici, ces sémaphores sont nommés d'après le produit, suffixé par
 * ".xcl" pour le premier et ".att" pour le second.
 *
 * Dans le premier schéma de synchronisation, chaque client qui
 * ne peut acheter le produit incrémente le nombre N de clients
 * en attente dans le fichier, puis fait un P(att). Ainsi, le
 * vendeur qui réapprovisionne son étal fait N fois V(att).
 * Variante : on peut compter non pas le nombre de clients,
 * mais le nombre de produits demandés. C'est juste plus lent.
 *
 * Dans le deuxième schéma de synchronisation, le fichier ne
 * contient que le nombre de produits actuellement sur l'étal
 * (on n'y ajoute rien par rapport à l'énoncé). Le client non
 * satisfait effectue un P(att). Lorsqu'un vendeur réapprovisionne
 * l'étal, il fait V(att), charge au client débloqué de faire
 * V(att) pour débloquer les autres clients en attente.
 *
 * Entre les deux schémas, il n'y a pas de meilleur dans l'absolu.
 * Le premier nécessite d'ajouter une information dans le fichier,
 * mais est plus robuste que le second si un client s'arrête
 * avant d'avoir fait son V(att). Dans le premier schéma, on est
 * sûr de réveiller des clients effectivement en attente. Dans
 * le deuxième schéma, selon l'implémentation, on peut avoir un
 * excès de vérifications dû au fait que le compteur du sémaphore
 * d'attente est incrémenté à chaque réassort sans qu'il soit
 * consommé par des P(att) de clients effectivement bloqués.
 *
 * Ici, le choix a été fait d'utiliser le premier schéma. Voir
 * la struct etal plus loin pour le format des informations dans
 * le fichier.
 */

#define	CHK(op)		do { if ((op) == -1) raler(1, #op); } while (0)
#define	SCHK(op)	do { if ((op) == SEM_FAILED) raler(1, #op); } while (0)

#define	NOM_PRODUIT	(10+1)			// y compris \0
#define	NOM_SEMAPHORE	(NOM_PRODUIT + 10)	// de la marge

/*
 * Pour grouper tous les accès aux différents éléments, on
 * définit une structure contenant tous les descripteurs ouverts.
 */

#define	SEM_XCL		0	// exclusion mutuelle pour l'accès au fichier
#define	SEM_ATT		1	// attente des clients

struct acces
{
    int fd ;			// accès au fichier
    sem_t *tabsem [2] ;		// les sémaphores
} ;

/*
 * Structure de données dans le fichier
 */

struct etal
{
    int nprod ;			// nombre de produits disponibles
    int natt ;			// nombre de clients en attente
} ;

/*
 * Définition des prototypes des fonctions de lib.c
 */

extern noreturn void raler (int syserr, const char *fmt, ...) ;
extern char **nom_semaphores (const char *produit) ;
extern void P (sem_t *sem) ;
extern void V (sem_t *sem) ;
extern struct acces open_etal (const char *produit, int creer) ;
extern void close_etal (struct acces *a) ;
extern void lire_etal (struct acces *a, struct etal *e) ;
extern void ecrire_etal (struct acces *a, struct etal *e) ;
extern void supprimer_etal (const char *produit) ;
