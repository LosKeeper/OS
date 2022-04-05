# OS
Cours sur l'architecture des systèmes d'exploitation

## Macros Utilisées
Pour l'ensemble des exercices, on utilisera le fichier `macros.h` qui contient les macros suivantes : 
- **`raler(int n,char *message)`** : affiche `message` puis renvoie errno si `n` vaut 1 le tout en interrompant le programme en faisant un `exit(EXIT_FAILLURE)`.
- **`CHK(cmd)`** : execute `cmd` et affiche le message d'erreur si `cmd` échoue (à utiliser si `cmd` positionne errno).
- **`TCHK(pthread_cmd)`** : execute `pthread_cmd`, positionne errno et affiche le message d'erreur si `pthread_cmd` échoue.
## [TP Assembleur](https://github.com/LosKeeper/OS/blob/main/ex1/suj-exo1.pdf)
Exercice de conversion de fonctions simples de C à assembleur en [Zorglub33](https://github.com/sandhose/z33-emulator).

## [TP1](https://github.com/LosKeeper/OS/blob/main/tp1/suj-tp1.pdf)
- **Exercice 1** : Exercice de création de threads indépendants réalisant des calculs séparés.
- **Exercice 2** : Exercice d'affichage de lignes avec alternance des signes dans le terminal (en utilisant des threads avec des barrières).
- **Exercice 3** : Exercice réalisant les diférentes tâches modélisées par un graphe dans l'ordre indiqué et en parallèle.