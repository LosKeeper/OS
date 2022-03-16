main_calcul:
    push 1
    push 3
    call calcul
    reset

main_prodscal:
    push 3
    push p2
    push p1
    call prodscal
    reset

p1: 
    .word 1 
    .word 2 
    .word 3

p2: 
    .word 1 
    .word 2 
    .word 3

main_racine:
    push 12
    call racine
    reset

calcul:
    push %b

    ld [%sp+2],%a   // a <- x
    ld [%sp+3],%b   // b <- y
    mul %b,%a       // a <- xy
    mul 2,%a        // a <- 2xy
    neg %a          // a <- -2xy
    mul %b,%b       // b <- y*y
    sub %b,%a       // a <- -2xy-y*y
    ld [%sp+2],%b   // b <- x
    mul %b,%b       // b <- x*x
    add %b,%a       // a <- -2xy-y*y+x*x

    pop %b
    rtn

last_arg:
    push %b

    ld 9998,%b      // b <- 9998 qui correspond a 10000 qui est la valeur 
                    // max de la pile a laquelle on a soustrait le premier 
                    // push b et et l'argument de retour de fonction

    sub %sp,%b      // b recoit le nombre total d'arguments de la 
                    // fonction envoyée

    add %b,%sp      // on descends la pile jusqu'à la derniere valeur de 
                    // la pile

    ld [%sp+1],%a   // a recoit le dernier argument de la fonction

    sub %b,%sp      // on remonte la pile a la valeur de base

    pop %b
    rtn

prodscal:
    push %b

    push 0      // on place p=0
    push 0      // on place i=0
    jmp p_condition

p_condition:
    ld [%sp],%b     // b <- i
    ld [%sp+6],%a   // a <- n
    cmp %b,%a
    jge p_end_for   // on saute a la fin de la boucle for si i >=n
    ld [%sp+4],%a   // a recoit l'adresse initiale du premier tableau
    add %b,%a       // a recoit l'adresse de la veleur en i du premier tableau
    ld [%a],%a      // a <- v1[i]
    push %a         // on place la valeur courante sur la pile
    ld [%sp+6],%a   // a recoit l'adresse initiale du second tableau
    add %b,%a       // a recoit l'adresse de la veleur en i du second tableau
    ld [%a],%a      // a <- v2[i]
    pop %b          // b <- v1[i]
    mul %b,%a       // a <- v1[i]*v2[i]
    ld [%sp+1],%b   // b <- p
    add %a,%b       // b <- p + v1[i]*v2[i]
    st %b,[%sp+1]   // p <- p + v1[i]*v2[i]
    ld [%sp],%b     // b <- i
    add 1,%b        // b <- i+1
    st %b,[%sp]     // i <- i+1
    jmp p_condition

p_end_for:
    add 1,%sp       // on depile i
    pop %a          // a <- p
    pop %b
    rtn

racine:
    push %b

    push 1                      // on place inf=1 sur la pile
    ld [%sp+3],%b               // b <- n
    push %b                     // on place sup=n sur la pile
    ld [%sp],%a                 // a <- sup
    sub 1,%a                    // a <- sup-inf
    div 2,%a                    // a <- (sup-inf)/2
    add 1,%a                    // a <- inf+(sup-inf)/2
    push %a                     // on place r sur la pile
    jmp r_while_first_condition // on verifie la premiere condition du while

r_while_first_condition:
    ld [%sp+5],%a                   // a <- n
    ld [%sp],%b                     // b <- r
    mul %b,%b                       // b <- r*r
    cmp %b,%a
    jgt r_while                     // saut dans le while si r*r > n
    jmp r_while_seconde_condition   // sinon on verifie la seconde condition
    
r_while_seconde_condition:
    ld [%sp],%b                 // b <- r
    add 1,%b                    // b <- r+1
    mul %b,%b                   // b <- (r+1)*(r+1)
    cmp %b,%a
    jle r_while                 // saut dans le while si (r+1)*(r+1) <= n

    jmp r_while_end             // sinon on ne rentre pas dans le while et on
                                // se dirige a la fin de la fonction

r_while:                            
    ld [%sp],%b                     // b <- r
    mul %b,%b                       // b <- r*r
    ld [%sp+5],%a                   // a <- n
    cmp %b,%a
    jle r_else                      // saut dans le else si r*r <= n
                                    // sinon on continue dans la fonction

    ld [%sp],%b                     // b <-r
    st %b,[%sp+1]                   // sup <- r
    ld [%sp+1],%a                   // a <- sup
    ld [%sp+2],%b                   // b <- inf
    sub %b,%a                       // a <- sup-inf
    div 2,%a                        // a <- (sup-inf)/2
    add %b,%a                       // a <- inf+(sup-inf)/2 
    st %a,[%sp]                     // r <- inf+(sup-inf)/2 
    jmp r_while_first_condition     // on recommence le while en reverifiant 
                                    // les conditions du while

r_else:
    ld [%sp],%b                     // b <- r
    st %b,[%sp+2]                   // inf <- r
    ld [%sp+1],%a                   // a <- sup
    sub %b,%a                       // a <- sup-inf
    div 2,%a                        // a <- (sup-inf)/2
    add %b,%a                       // a <- inf+(sup-inf)/2 
    st %a,[%sp]                     // r <- inf+(sup-inf)/2 
    jmp r_while_first_condition     // on recommence le while en reverifiant 
                                    // les conditions du while

r_while_end: 
    pop %a                          // a <- r
    add 2,%sp                       // on retire inf et sup de la pile

    pop %b
    rtn