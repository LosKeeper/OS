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
    push 100
    call racine
    reset

calcul:
    push %b
    ld [%sp+2],%a
    ld [%sp+3],%b
    mul %b,%a
    mul 2,%a
    neg %a
    mul %b,%b
    sub %b,%a
    ld [%sp+2],%b
    mul %b,%b
    add %b,%a
    pop %b
    rtn

last_arg:
    push %b
    ld 9998,%b
    sub %sp,%b
    add %b,%sp
    ld [%sp+1],%a
    sub %b,%sp
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
    add %a,%b
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
    push 1
    ld [%sp+3],%b
    push %b
    ld [%sp],%a
    sub 1,%a
    div 2,%a
    add 1,%a
    push %a
    jmp r_while_first_condition

r_while_first_condition:
    ld [%sp+5],%a
    ld [%sp],%b
    mul %b,%b
    cmp %b,%a
    jgt r_while
    jmp r_while_seconde_condition
    
r_while_seconde_condition:
    ld [%sp],%b
    add 1,%b
    mul %b,%b
    cmp %b,%a
    jle r_while
    jmp r_while_end

r_while:
    ld [%sp],%b
    mul %b,%b
    ld [%sp+5],%a
    cmp %b,%a
    jle r_else
    ld [%sp],%b
    st %b,[%sp+1]
    ld [%sp+1],%a
    ld [%sp+2],%b
    sub %b,%a
    div 2,%a
    add %b,%a
    st %a,[%sp]
    jmp r_while_first_condition

r_else:
    ld [%sp],%b
    st %b,[%sp+2]
    ld [%sp+1],%a
    sub %b,%a
    div 2,%a
    add %b,%a
    st %a,[%sp]
    jmp r_while_first_condition

r_while_end: 
    pop %a
    add 1,%sp
    add 1,%sp
    pop %b
    rtn