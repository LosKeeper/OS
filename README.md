# [OS](https://pdagog.gitlab.io/ens/cours-ase.pdf)
Cours sur l'architecture des systèmes d'exploitation.

## Macros utilisées
Pour l'ensemble des exercices, on utilisera le fichier [`macro.h`](macro.h) qui contient les macros suivantes : 
- **`raler(int n,char *message)`** : affiche `message` puis renvoie errno si `n` vaut 1 le tout en interrompant le programme en faisant un `exit(EXIT_FAILLURE)`.
- **`CHK(cmd)`** : execute `cmd` et affiche le message d'erreur si `cmd` échoue (à utiliser si `cmd` positionne errno).
- **`TCHK(pthread_cmd)`** : execute `pthread_cmd`, positionne errno et affiche le message d'erreur si `pthread_cmd` échoue.
## [TP Assembleur](TP_Assembleur/suj-exo1.pdf)
- Exercice de conversion de fonctions simples de C à assembleur en [Zorglub33](https://github.com/sandhose/z33-emulator).
> Pour compiler le code assembleur, il faut utiliser le compilateur [`z33-cli`](TP_Assembleur/z33-cli) qui est déjà installé sur le repo et donc les principales commandes sont référencées [ici](https://pdagog.gitlab.io/ens/z33refcard.pdf).

## [TP1](TP1/suj-tp1.pdf)
- [**Exercice 1**](TP1/ex1.c) : Exercice de création de threads indépendants réalisant des calculs séparés.
- [**Exercice 2**](TP1/ex2.c) : Exercice d'affichage de lignes avec alternance des signes dans le terminal (en utilisant des threads avec des barrières).
- [**Exercice 3**](TP1/ex3.c) : Exercice réalisant les diférentes tâches modélisées par un graphe dans l'ordre indiqué et en parallèle.

## [TP2](TP2/suj-tp2.pdf)
- [**Exercice 1**](TP2/ex1/ex1_2.c) : Exercice utilisant des mutex avec probleme de concurrence
- [**Exercice 2**](TP2/ex2/ex2.c) : Exercice utilisant des sémaphores

## [TP3](TP3/suj-tp3.pdf)
- [**Exercice 1**](TP3/ex1.c) : Exercice d'utilisation de conditions pour synchroniser les threads simulant la gestion d'un centre de calcul.
- [**Exercice 2**](TP3/ex2.c) : Exercice d'utilisation de conditions et sémaphores pour synchroniser les threads simulant un système avec des cuisiniers et un épicier livrant des ingrédients pour faire des gâteaux. 
    > Exercice 2 en cours de résolution

## Annales
### [TP 2020-2021](Annales/20-21/suj-cc.pdf)
- [**Exercice**](Annales/20-21/tp21-21.c) : Système multithreads.

### [TP 2019-2020](Annales/19-20/suj-cc.pdf)
- [**Exercice**](Annales/19-20/tp19-20.c) : Système producteur consommateur à tampon borné.
