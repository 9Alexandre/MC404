.global ajudaORobinson

.data

output_buffer:  .skip 32

.align 4

ajudaORobinson:

	@ Marca todas as posicoes como nao visitadas
	bl inicializaVisitados

	@ Encontra a posicao inicial de Robinson
	bl posicaoXRobinson
	mov r1, r0
	bl posicaoYRobinson
	mov r2, r0
	
	mov r0, r1
	mov r1, r2

	push {r0, r1}
	@ Marca posicao inicial como visitada
	bl visitaCelula
	pop {r0, r1}

	@ Imprime posicao inicial
	bl imprime

	push {r0, r1}
	@ Encontra a posicao do local 
	bl posicaoXLocal
	mov r2, r0
	bl posicaoYLocal
	mov r3, r0
	pop {r0, r1}

	@ Chama a funcao que procura os caminhos possiveis
	bl caminho

	@ Chama a funcao exit para finalizar processo.
    mov r0, #0
    bl  exit

@ r0: coordenada x do Robinson
@ r1: coordenada y do Robinson
@ r2: coordenada x do local
@ r3: coordenada y do local

caminho:
	push {r0-r5, lr}
	mov r4, r0	@ r4: coordenada x do Robinson
	mov r5, r1	@ r5: coordenada y do Robinson

dir:
	add r4, r4, #1
	cmp r4, r2 @ Verifica se a posicao final foi encontrada
	bne d
	cmp r5, r3
	beq end    @ Se sim, fim da funcao, se nao, ela continua
d:
	mov r0, r4 @ Parametros para "daParaPassar"
	mov r1, r5
	bl daParaPassar
	cmp r0, #0
	subeq r4, r4, #1
	beq esq
	mov r0, r4	@ Parametros para "foiVisitado"
	mov r1, r5
	bl foiVisitado
	cmp r0, #1
	subeq r4, r4, #1
	beq esq
	mov r0, r4	@ Parametros para "visitaCelula"
	mov r1, r5
	bl visitaCelula
	mov r0, r4
	mov r1, r5
	bl imprime
	bl caminho

esq:
	sub r4, r4, #1
	cmp r4, r2 @ Verifica se a posicao final foi encontrada
	bne e
	cmp r5, r3
	beq end    @ Se sim, fim da funcao, se nao, ela continua
e:
	mov r0, r4 @ Parametros para "daParaPassar" 
	mov r1, r5
	bl daParaPassar
	cmp r0, #0
	addeq r4, r4, #1
	beq cima
	mov r0, r4	@Parametros para "foiVisitado"
	bl foiVisitado
	cmp r0, #1
	addeq r4, r4, #1
	beq cima
	mov r0, r4	@Parametros para "visitaCelula"
	bl visitaCelula
	mov r0, r4
	mov r1, r5
	bl imprime
	bl caminho

cima:
	add r5, r5, #1
	cmp r4, r2 @ Verifica se a posicao final foi encontrada
	bne c
	cmp r5, r3
	beq end    @ Se sim, fim da funcao, se nao, ela continua
c:
	mov r0, r4 @ Parametros para "daParaPassar" 
	mov r1, r5
	bl daParaPassar
	cmp r0, #0
	subeq r5, r5, #1
	beq baixo
	mov r0, r4	@Parametros para "foiVisitado"
	bl foiVisitado
	cmp r0, #1
	subeq r5, r5, #1
	beq baixo
	mov r0, r4	@Parametros para "visitaCelula"
	bl visitaCelula
	mov r0, r4
	mov r1, r5
	bl imprime
	bl caminho

baixo:
	sub r5, r5, #1
	cmp r4, r2	 @ Verifica se a posicao final foi encontrada
	bne d
	cmp r5, r3
	beq end   	 @ Se sim, fim da funcao, se nao, ela continua
b:
	mov r0, r4 @ Parametros para "daParaPassar" 
	mov r1, r5
	bl daParaPassar
	cmp r0, #0
	addeq r5, r5, #1
	beq end
	mov r0, r4	@Parametros para "foiVisitado"
	bl foiVisitado
	cmp r0, #1
	addeq r5, r5, #1
	beq end	
	mov r0, r4	@Parametros para "visitaCelula"
	bl visitaCelula
	mov r0, r4
	mov r1, r5
	bl imprime
	bl caminho

end:
	pop {r0-r5, lr}
	mov pc, lr

@ Imprime o par de coordenadas
imprime:
	push {r0-r4, lr}
	mov r2, r1
	mov r1, r0
	ldr r0, =output_buffer

	add r1, r1, #48		@ x
	strb r1, [r0, #0]
	mov r3, #' '		@ espaço
	strb r3, [r0, #1]
	add r2, r2, #48		@ y
	strb r2, [r0, #2]
	mov r3, #'\n'		@ \n
	strb r3, [r0, #3]

	@ Chama a funcao write para escrevero o 
	@ par de coordemanda "x y"
    mov r1, #4        
    bl  write
	pop {r0-r4, lr}
	mov pc, lr

@ Escreve uma sequencia de bytes na saida padrao.
@ parametros:
@  r0: endereco do buffer de memoria que contem a sequencia de bytes.
@  r1: numero de bytes a serem escritos
write:
    push {r0-r7, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #1         @ stdout file descriptor = 1
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho do buffer.
    mov r7, #4         @ write
    svc 0x0
    pop {r0-r7, lr}
    mov pc, lr

@ Finaliza a execucao de um processo.
@ r0: codigo de finalizacao (Zero para finalizacao correta)
exit:    
    mov r7, #1         @ syscall number for exit
    svc 0x0	
