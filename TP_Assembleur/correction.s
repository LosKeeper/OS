calcul:
	// à l'entrée dans la fonction, on a : x:[%sp+1], y:[%sp+2]
	push %b			// x:[%sp+2], y:[%sp+3]
	ld   [%sp+2],%a		// a <- x
	mul  %a,%a		// a <- x^2
	ld   [%sp+2],%b
	mul  2,%b
	mul  [%sp+3],%b		// b <- 2xy
	sub  %b,%a		// a <- x^2 - 2xy
	ld   [%sp+3],%b
	mul  %b,%b		// b <- y^2
	sub  %b,%a		// a <- x^2 - 2xy - y^2
	pop  %b
	rtn

main_calcul:
	push 3			// y
	push 9			// x
	call calcul
	add  2,%sp
	reset			// on attend 18 dans a et 10000 dans sp

///////////////////////////////////////////////////////////////////

prodscal:
	// à l'entrée dans la fonction, on a :
	// &v1[0]:[%sp+1], &v2[0]:[%sp+2], n:[%sp+3]
	push %b			// svg b: sp, v1: sp+2, v2: sp+3, n: sp+4
	push 0			// p:sp, svg b:sp+1, v1:sp+3, v2:sp+4, n:sp+5
	// on réalise un saut direct au test : ainsi, la plupart des
	// tours de boucle n'ont qu'une seule rupture de séquence
	jmp  prodscal_fin_boucle

prodscal_deb_boucle:
	// à chaque tour de boucle, la pile est dans l'état suivant :
	//	p:sp, svg b:sp+1, v1:sp+3, v2:sp+4, n:sp+5
	sub  1,%a		// n--
	st   %a,[%sp+5]

	ld   [%sp+3],%a
	ld   [%a],%a		// a <- *v1
	ld   [%sp+4],%b
	ld   [%b],%b		// b <- *v2
	mul  %b,%a		// a <- *v1 * *v2
	ld   [%sp],%b
	add  %a,%b		// b <- p + *v1 * *v2
	st   %b,[%sp]		// p mis à jour

	ld   [%sp+3],%a
	add  1,%a
	st   %a,[%sp+3]		// v1++

	ld   [%sp+4],%a
	add  1,%a
	st   %a,[%sp+4]		// v2++

prodscal_fin_boucle:
	ld   [%sp+5],%a		// a <- n
	cmp  0,%a
	jlt  prodscal_deb_boucle

	pop  %a			// a <- p
	pop  %b			// restaurer b
	rtn

	// deux tableaux de 3 éléments
v1:
	.word 3
	.word 5
	.word 7
v2:
	.word 4
	.word 6
	.word 8

main_prodscal:
	push 3
	push v2
	push v1
	call prodscal
	add  3,%sp
	reset			// on attend 98 dans a et 10000 dans sp

///////////////////////////////////////////////////////////////////

racine:
	// à l'entrée dans la fonction, on a : n:sp+1
	push %b			// svgb:sp, n:sp+2
	push 1			// inf:sp, svgb:sp+1, n:sp+3
	ld   [%sp+3],%a
	push %a			// sup:sp, inf:sp+1, svgb:sp+2, n:sp+4
	ld   1,%b
	push 0			// r:sp, sup:sp+1, inf:sp+2, svgb:sp+3, n:sp+5
	jmp  racine_fin_boucle

racine_deb_boucle:
	// À chaque tour de boucle, on a :
	//	pile :  r:sp, sup:sp+1, inf:sp+2, svgb:sp+3, n:sp+5
	//	registre a : r^2
	// On fait ici du code naïf : on refait le test r*r > n
	ld   [%sp],%b		// b <- r
	cmp  [%sp+5],%a
	jlt  racine_if_then
	st   %b,[%sp+2]		// inf <- r
	jmp  racine_fin_if

racine_if_then:
	st   %b,[%sp+1]		// sup <- r
racine_fin_if:			// les étiquettes peuvent être consécutives

racine_fin_boucle:
	// calcul de r = inf + (sup-inf)/2
	ld   [%sp+1],%a		// a <- sup
	ld   [%sp+2],%b		// b <- inf
	sub  %b,%a		// a <- sup-inf
	div  2,%a		// a <- (sup-inf)/2
	add  [%sp+2],%a		// a <- inf + (sup-inf)/2
	st   %a,[%sp]		// nouvelle valeur de r

	ld   %a,%b		// b <- r (sauvegarde)
	mul  %a,%a		// a <- r^2
	cmp  [%sp+5],%a
	jlt  racine_deb_boucle

	add  1,%b		// b <- r+1
	mul  %b,%b		// b <- (r+1)^2
	cmp  [%sp+5],%b
	jge  racine_deb_boucle

	ld   [%sp],%a		// a <- r
	add  3,%sp		// dépiler r, sup, inf
	pop  %b			// restaurer b
	rtn

main_racine:
	push 145
	call racine
	add  1,%sp
	reset			// on attend 12 dans a et 10000 dans sp