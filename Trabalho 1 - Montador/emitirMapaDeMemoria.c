/*
Trabalho 1: Montador para a arquitetura do computador IAS
Parte 2: Emissão do Mapa de Memória
*/

#include "montador.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum lado {Esquerda, Direita} lado;

typedef struct Linha{

	int pos;
	char comandoD[2], comandoE[2];
	int memoriaD, memoriaE;
	char constante[10];
	char *rotuloE, *rotuloD;
} linha;

typedef struct DefRot{

	char *nome;
	lado lado;
	int valor;
} defRot;

void instrucoesE(linha comandos[], int linhaAtual, int i);
void instrucoesD(linha comandos[], int linhaAtual, int i);
void converterDec(int n, char hex[]);
int rotulos(linha comandos[], int tamTab,int linhaAtual, defRot tabela[]);
int completa(linha comandos[], int linhaAtual);
int preencheWfill(int pos, int tamTab, int linhaAtual, linha comandos[], char *simbolo, defRot tabela[]);
int preencheWord(int pos, int tamTab, int linhaAtual, linha comandos[], char *simbolo, defRot tabela[]);

/* Retorna:
 *  1 caso haja erro na montagem;
 *  0 caso não haja erro.
 */

int emitirMapaDeMemoria(){

	int i, j=0, k=0, n; // Contadores
	int linhaAtual = 0, tamTab=0;
	int linhaWfill=0, wfill=-1;
	int linhaWord=0, word = -1;
	char *simbolo;
	lado Dir_Esq = Esquerda;
	defRot tabela[1024];
	linha comandos[1024];
	Token token;

	// Consulta toda a tabela de tokens
	// Realiza um tarefa especifica para cada tipo de token
	for(i=0; i<getNumberOfTokens(); i++){
		token = recuperaToken(i);

		// Instruções
		if(token.tipo == Instrucao){
			if(Dir_Esq == Esquerda){
				instrucoesE(comandos, linhaAtual, i);
				Dir_Esq = Direita;
			}else{
				instrucoesD(comandos, linhaAtual, i);
				Dir_Esq = Esquerda;
				linhaAtual++;
			}

		// Diretivas
		}else if(token.tipo == Diretiva){

			// .set
			if(token.palavra[1] == 's'){
			token = recuperaToken(i+1);
				tabela[tamTab].nome = token.palavra;

				token = recuperaToken(i+2);
				if(token.tipo == Hexadecimal)
					tabela[tamTab].valor = strtol(token.palavra, NULL, 16);
				else if(token.tipo == Decimal)
					tabela[tamTab].valor = atoi(token.palavra);
				i = i+2;
				tamTab++;

			// .org
			}else if(token.palavra[1] == 'o'){
				token = recuperaToken(i+1);
				if(token.tipo == Hexadecimal)
					n = strtol(token.palavra, NULL, 16);
				else if(token.tipo == Decimal)
					n = atoi(token.palavra);
				else if(token.tipo == Nome){
					j=0;
					while(strcmp(tabela[j].nome, token.palavra))
						j++;

					n = tabela[j].valor;
				}

				for(j=0; j<linhaAtual; j++){
					if(comandos[j].pos == n){
						fprintf(stderr, "Impossível montar o código!\n");
						return 1;
					}
				}

				linhaAtual = n;
				i++;

			// .align
			}else if(token.palavra[1] == 'a'){
				token = recuperaToken(i+1);

				if(Dir_Esq == Direita){
					comandos[linhaAtual].comandoD[0] = '0';
					comandos[linhaAtual].comandoD[1] = '0';

					linhaAtual++;
					while(linhaAtual % atoi(token.palavra) != 0)
						linhaAtual++;
						Dir_Esq = Esquerda;

				}else{
					while(linhaAtual % atoi(token.palavra) != 0)
						linhaAtual++;
				}
				i++;

			// .wfill
			}else if(token.palavra[2] == 'f'){
				if(Dir_Esq != Esquerda){
					fprintf(stderr, "Impossível montar o código!\n");
					return 1;
				}else{
					token = recuperaToken(i+1);
					n = atoi(token.palavra);
					token = recuperaToken(i+2);

					if(token.tipo == Hexadecimal){
						for(j=0; j<n; j++){
							for(k=0; k<12; k++)
									comandos[linhaAtual].constante[k] = token.palavra[k+2];

								comandos[linhaAtual].pos = linhaAtual;
								linhaAtual++;
						}
					}else if(token.tipo == Decimal){
						for(j=0; j<n; j++){
							converterDec(atoi(token.palavra), comandos[linhaAtual].constante);
							comandos[linhaAtual].pos = linhaAtual;
							linhaAtual++;
						}
					}else if(token.tipo == Nome){
						token = recuperaToken(i+2);
						simbolo = token.palavra;
						wfill = i;
						linhaWfill = linhaAtual;
						linhaAtual = linhaAtual + n;
					}
					i = i+2;
				}

			// .word
			}else if(token.palavra[2] == 'o'){
				if(Dir_Esq != Esquerda){
					fprintf(stderr, "Impossível montar o código!\n");
					return 1;
				}else{
					token = recuperaToken(i+1);

					if(token.tipo == Hexadecimal){
						for(k=0; k<12; k++)
								comandos[linhaAtual].constante[k] = token.palavra[k+2];

					}else if(token.tipo == Decimal){
						converterDec(atoi(token.palavra), comandos[linhaAtual].constante);

					}else if(token.tipo == Nome){
						simbolo = token.palavra;
						word = i;
						linhaWord = linhaAtual;
					}

					comandos[linhaAtual].pos = linhaAtual;
					linhaAtual++;
					i++;
				}
			}

		// Definição de Rótulo
		}else if(token.tipo == DefRotulo){
			tabela[tamTab].nome = token.palavra;
			j = strlen(tabela[tamTab].nome);
			tabela[tamTab].nome[j-1] = '\0'; // Rerira o ':'
			tabela[tamTab].lado = Dir_Esq;
			tabela[tamTab].valor = linhaAtual;
			tamTab++;
		}
	}

	// Caso o mapa de memória termine na palavra da esquerda
	// a palavra da direita é completada com zeros
	if(Dir_Esq == Direita){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '0';
		comandos[linhaAtual].memoriaD = 0;
		linhaAtual++;
	}

	// Liga a instrução com o valor da memoria armazenada no seu respectivo rótulo
	if(rotulos(comandos, tamTab, linhaAtual, tabela))
		return 1;

	// Completa as instruções faltantes
	completa(comandos, linhaAtual);

	// Preenche o mapa de memória com as posições definidas com wfill
	// com o valor definido por um rótulo
	if(wfill > -1){
		if(preencheWfill(wfill, tamTab, linhaWfill, comandos, simbolo, tabela))
			return 1;
	}

	// Preenche uma posição de memoria com o valor definido por um rótulo
	if(word > -1){
		if(preencheWord(word, tamTab, linhaWord, comandos, simbolo, tabela))
			return 1;
	}

	// Imprime o mapa de memória
	for(i=0; i<linhaAtual; i++){
		if(strlen(comandos[i].constante) > 0 || strlen(comandos[i].comandoE) > 0 ){

			// Imprime as posição de cada palavra de memória
			printf("%03X ", comandos[i].pos);

			// Imprime as constante, caso haja
			if(strlen(comandos[i].constante) > 0){
				for(j=0; j<10; j++){
					if(j == 2 || j == 5 || j == 7)
						printf(" ");
						printf("%c", comandos[i].constante[j]);
					}
					printf("\n");
			}else{
				// Imprime as instruções e as posicões de memória a elas correspondentes
				printf("%c%c %03X ", comandos[i].comandoE[0], comandos[i].comandoE[1], comandos[i].memoriaE);
				printf("%c%c %03X\n", comandos[i].comandoD[0], comandos[i].comandoD[1], comandos[i].memoriaD);
			}
		}
	}

	return 0;
}

// Atribui a instrução da palavra de memória da esquerda
void instrucoesE(linha comandos[], int linhaAtual, int i){

	Token token;

	comandos[linhaAtual].pos = linhaAtual;
	token = recuperaToken(i);

	if(!strcmp(token.palavra, "LOAD")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '1';

	}else if(!strcmp(token.palavra, "LOAD-")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '2';

	}else if(!strcmp(token.palavra, "LOAD|")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '3';

	}else if(!strcmp(token.palavra, "LOADmq")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = 'A';

	}else if(!strcmp(token.palavra, "LOADmq_mx")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '9';

	}else if(!strcmp(token.palavra, "STOR")){
		comandos[linhaAtual].comandoE[0] = '2';
		comandos[linhaAtual].comandoE[1] = '1';

	}else if(!strcmp(token.palavra, "JUMP")){
		comandos[linhaAtual].comandoE[0] = 'J';
		comandos[linhaAtual].comandoE[1] = 'J';

	}else if(!strcmp(token.palavra, "JMP+")){
		comandos[linhaAtual].comandoE[0] = '+';
		comandos[linhaAtual].comandoE[1] = '+';

	}else if(!strcmp(token.palavra, "ADD")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '5';

	}else if(!strcmp(token.palavra, "ADD|")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '7';

	}else if(!strcmp(token.palavra, "SUB")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '6';

	}else if(!strcmp(token.palavra, "SUB|")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = '8';

	}else if(!strcmp(token.palavra, "MUL")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = 'B';

	}else if(!strcmp(token.palavra, "DIV")){
		comandos[linhaAtual].comandoE[0] = '0';
		comandos[linhaAtual].comandoE[1] = 'C';

	}else if(!strcmp(token.palavra, "LSH")){
		comandos[linhaAtual].comandoE[0] = '1';
		comandos[linhaAtual].comandoE[1] = '4';

	}else if(!strcmp(token.palavra, "RSH")){
		comandos[linhaAtual].comandoE[0] = '1';
		comandos[linhaAtual].comandoE[1] = '5';

	}else if(!strcmp(token.palavra, "STORA")){
		comandos[linhaAtual].comandoE[0] = 'S';
		comandos[linhaAtual].comandoE[1] = 'S';
	}

	if(strcmp(token.palavra, "RSH") != 0 && strcmp(token.palavra, "LSH") != 0){
		token = recuperaToken(i+1);
		if(token.tipo == Hexadecimal)
			comandos[linhaAtual].memoriaE = strtol(token.palavra, NULL, 16);
		else if(token.tipo == Decimal)
			comandos[linhaAtual].memoriaE = atoi(token.palavra);
		else if(token.tipo == Nome)
			comandos[linhaAtual].rotuloE = token.palavra;
		}
}

// Atribui a instrução da palavra de memória da direita
void instrucoesD(linha comandos[], int linhaAtual, int i){

	Token token;

	comandos[linhaAtual].pos = linhaAtual;
	token = recuperaToken(i);

	if(!strcmp(token.palavra, "LOAD")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '1';

	}else if(!strcmp(token.palavra, "LOAD-")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '2';

	}else if(!strcmp(token.palavra, "LOAD|")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '3';

	}else if(!strcmp(token.palavra, "LOADmq")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = 'A';

	}else if(!strcmp(token.palavra, "LOADmq_mx")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '9';

	}else if(!strcmp(token.palavra, "STOR")){
		comandos[linhaAtual].comandoD[0] = '2';
		comandos[linhaAtual].comandoD[1] = '1';

	}else if(!strcmp(token.palavra, "JUMP")){
		comandos[linhaAtual].comandoD[0] = 'J';
		comandos[linhaAtual].comandoD[1] = 'J';

	}else if(!strcmp(token.palavra, "JMP+")){
		comandos[linhaAtual].comandoD[0] = '+';
		comandos[linhaAtual].comandoD[1] = '+';

	}else if(!strcmp(token.palavra, "ADD")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '5';

	}else if(!strcmp(token.palavra, "ADD|")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '7';

	}else if(!strcmp(token.palavra, "SUB")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '6';

	}else if(!strcmp(token.palavra, "SUB|")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = '8';

	}else if(!strcmp(token.palavra, "MUL")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = 'B';

	}else if(!strcmp(token.palavra, "DIV")){
		comandos[linhaAtual].comandoD[0] = '0';
		comandos[linhaAtual].comandoD[1] = 'C';

	}else if(!strcmp(token.palavra, "LSH")){
		comandos[linhaAtual].comandoD[0] = '1';
		comandos[linhaAtual].comandoD[1] = '4';

	}else if(!strcmp(token.palavra, "RSH")){
		comandos[linhaAtual].comandoD[0] = '1';
		comandos[linhaAtual].comandoD[1] = '5';

	}else if(!strcmp(token.palavra, "STORA")){
		comandos[linhaAtual].comandoD[0] = 'S';
		comandos[linhaAtual].comandoD[1] = 'S';
	}

	if(strcmp(token.palavra, "RSH") != 0 && strcmp(token.palavra, "LSH") != 0){
		token = recuperaToken(i+1);
		if(token.tipo == 3)
			comandos[linhaAtual].memoriaD = strtol(token.palavra, NULL, 16);
		else if(token.tipo == 4)
			comandos[linhaAtual].memoriaD = atoi(token.palavra);
		else if(token.tipo == 5)
			comandos[linhaAtual].rotuloD = token.palavra;
		}
}

// Converte um número decimal para hexadecimal
void converterDec(int n, char hex[]){

		int i=0, j, k=9, resto[10];

		while(n > 0){
			resto[i] = n % 16;
			n = n / 16;
			i++;
		}

		for(j=0; j<10; j++)
			hex[j] = '0';

		for(j=0; j<i; j++){
			if(resto[j] == 10)
				hex[k] = 'A';
			else if(resto[j] == 11)
				hex[k] = 'B';
			else if(resto[j] == 12)
				hex[k] = 'C';
			else if(resto[j] == 13)
				hex[k] = 'D';
			else if(resto[j] == 14)
				hex[k] = 'E';
			else if(resto[j] == 15)
				hex[k] = 'F';
			else
				hex[k] = resto[j] + 48;
				k--;
		}
}

// Encontra a posição de memória conrrespondente ao rótulo e a insere no mapa de memória
int rotulos(linha comandos[], int tamTab,int linhaAtual, defRot tabela[]){

	int i, j, k;

	for(i=0; i<linhaAtual; i++){

		// Palavra de memória da esquerda
		if(comandos[i].rotuloE != NULL){
			k=0;
			for(j=0; j<tamTab; j++){
				if(strcmp(comandos[i].rotuloE, tabela[j].nome) == 0){
				k++;
					break;
				}
			}

			if(k != 0)
				comandos[i].memoriaE = tabela[j].valor;
			else{
				printf("USADO MAS NÃO DEFINIDO: %s!\n", comandos[i].rotuloE);
				return 1;
			}

			if(comandos[i].comandoE[0] == 'J'){
				if(tabela[j].lado == Esquerda){
						comandos[i].comandoE[0] = '0';
						comandos[i].comandoE[1] = 'D';
				}else{
					comandos[i].comandoE[0] = '0';
					comandos[i].comandoE[1] = 'E';
					}
			}else if(comandos[i].comandoD[0] == '+'){
				if(tabela[j].lado = Esquerda){
					comandos[i].comandoE[0] = '0';
					comandos[i].comandoE[1] = 'F';
				}else{
					comandos[i].comandoE[0] = '1';
					comandos[i].comandoE[1] = '0';
				}
			}else if(comandos[i].comandoE[0] == 'S'){
				if(tabela[j].lado = Esquerda){
				comandos[i].comandoE[0] = '1';
				comandos[i].comandoE[1] = '2';
				}else{
					comandos[i].comandoE[0] = '1';
					comandos[i].comandoE[1] = '3';
				}
			}
		}

		// Palavra de memória da direita
		if(comandos[i].rotuloD != NULL){
			k=0;
			for(j=0; j<tamTab; j++){
				if(strcmp(comandos[i].rotuloD, tabela[j].nome) == 0){
					k++;
					break;
				}
			}

			if(k != 0)
				comandos[i].memoriaD = tabela[j].valor;
			else{
				printf("USADO MAS NÃO DEFINIDO: %s!\n", comandos[i].rotuloD);
				return 1;
			}

			if(comandos[i].comandoD[0] == 'J'){
				if(tabela[j].lado == Esquerda){
					comandos[i].comandoD[0] = '0';
					comandos[i].comandoD[1] = 'D';
				}else{
					comandos[i].comandoD[0] = '0';
					comandos[i].comandoD[1] = 'E';
				}
			}else if(comandos[i].comandoD[0] == '+'){
				if(tabela[j].lado = Esquerda){
					comandos[i].comandoD[0] = '0';
					comandos[i].comandoD[1] = 'F';
				}else{
					comandos[i].comandoD[0] = '1';
					comandos[i].comandoD[1] = '0';
				}
			}else if(comandos[i].comandoD[0] == 'S'){
				if(tabela[j].lado = Esquerda){
					comandos[i].comandoD[0] = '1';
					comandos[i].comandoD[1] = '2';
				}else{
					comandos[i].comandoD[0] = '1';
					comandos[i].comandoD[1] = '3';
				}
			}
		}
	}

	return 0;
}

// Completas as instruções faltasntes
int completa(linha comandos[], int linhaAtual){

	int i;

	for(i=0; i<linhaAtual; i++){
		if(comandos[i].comandoE[0] == 'J'){
			comandos[i].comandoE[0] = '0';
			comandos[i].comandoE[1] = 'D';
		}else if(comandos[i].comandoD[0] == '+'){
			comandos[i].comandoE[0] = '0';
			comandos[i].comandoE[1] = 'F';
		}else if(comandos[i].comandoE[0] == 'S'){
			comandos[i].comandoE[0] = '1';
			comandos[i].comandoE[1] = '2';
		}

		if(comandos[i].comandoD[0] == 'J'){
			comandos[i].comandoD[0] = '0';
			comandos[i].comandoD[1] = 'D';
		}else if(comandos[i].comandoD[0] == '+'){
			comandos[i].comandoD[0] = '0';
			comandos[i].comandoD[1] = 'F';
		}else if(comandos[i].comandoE[0] == 'S'){
			comandos[i].comandoD[0] = '1';
			comandos[i].comandoD[1] = '2';
		}
	}
}

// Preeenche todas as posições definidas por um .wfill
int preencheWfill(int pos, int tamTab, int linhaAtual, linha comandos[], char *simbolo, defRot tabela[]){

	Token token;
	int i, j, k;

	token = recuperaToken(pos+1);
	k = atoi(token.palavra);

	token = recuperaToken(pos+2);

	j=0;
	for(i=0; i<tamTab; i++){
		if(strcmp(tabela[i].nome, token.palavra) == 0)
			j++;
			break;
	}

	if(j != 0){
		for(j=0; j<k; j++){
			sprintf(comandos[linhaAtual].constante, "%010X", tabela[i].valor);
			comandos[linhaAtual].pos = linhaAtual;
			linhaAtual++;
		}
	}else{
		token = recuperaToken(pos+2);
		printf("USADO MAS NÃO DEFINIDO: %s!\n", token.palavra);
		return 1;
	}

	return 0;
}

// Preeenche a posição definida por um .word
int preencheWord(int pos, int tamTab, int linhaAtual, linha comandos[], char *simbolo, defRot tabela[]){

	Token token;
	int i, j;

	token = recuperaToken(pos+1);

	j=0;
	for(i=0; i<tamTab; i++){
		if(strcmp(tabela[i].nome, token.palavra) == 0)
			j++;
			break;
	}

	if(j != 0){
			sprintf(comandos[linhaAtual].constante, "%010X", tabela[i].valor);
			comandos[linhaAtual].pos = linhaAtual;
	}else{
		printf("USADO MAS NÃO DEFINIDO: %s!\n", token.palavra);
		return 1;
	}

	return 0;
}
