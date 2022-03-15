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
    ld 3630,%b
    push 8
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
    add %a,%b
    add %b,%sp
    ld [%sp+1],%a
    sub %b,%sp
    ld [%sp+1],%b
    sub 3,%sp
    mul %b,%a
    pop %b
    add 1,%b
    push %b
    add [%sp+1],%a
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
    pop %b
    push %b
    mul %b,%b
    cmp %b,%a
    jgt r_while
    jmp r_while_seconde_condition
    
r_while_seconde_condition:
    pop %b
    push %b
    add 1,%b
    mul %b,%b
    cmp %b,%a
    jle r_while
    jmp r_while_end

r_while:
    pop %b
    push %b
    mul %b,%b
    cmp %b,%a
    jgt r_else
    pop %b
    push %b
    st %b,[%sp+2]
    jmp r_while_first_condition

r_else:
    pop %b
    push %b
    st %b,[%sp+3]
    ld [%sp+2],%a
    sub %b,%a
    div 2,%a
    add %b,%a
    pop %b
    push %a
    jmp r_while_first_condition

r_while_end: 
    pop %a
    add 1,%sp
    add 1,%sp
    pop %b
    rtn