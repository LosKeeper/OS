main_calcul:
    push 1
    push 3
    call calcul
    reset

main_prodscal:
    push 3
    push 1
    push 2
    push 3
    push 1
    push 2
    push 3
    call prodscal
    reset

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
    push 0 // p=0
    push 0 // i=0
    jmp p_for

p_for:
    ld [%sp],%b
    call last_arg
    cmp %b,%a
    jge p_end
    add 3,%sp
    add %b,%sp
    ld [%sp+1],%a
    add %a,%sp
    ld [%sp+1],%b
    push %b
    add 1,%sp
    call last_arg
    sub %a,%sp
    sub 3,%sp
    pop %b
    pop %a
    mul %b,%a
    pop %b
    sub %b,%sp
    add 1,%b
    push %b
    ld [%sp+1],%b
    add %b,%a
    st %a,[%sp+1]
    jmp p_for

p_end:
    add 1,%sp
    pop %a
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