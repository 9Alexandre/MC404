/*
Trabalho 1: Montador para a arquitetura do computador IAS
Parte 1: Processamento da Entrada
*/

#include "montador.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void separaTermos(char* frase, int linha);
void classifica(char* parametro, int linha);
char* ingnoraComentarios(char* frase);
int erros1(char* frase);
int erros2();

/*
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem;
 *  0 caso não haja erro.
 */

int processarEntrada(char* entrada, unsigned tamanho){

	int i=0, linhas=0;
	char* frase;

	// Separa o arquivo de entrada em linhas
	// Procura erros
	// Ingnorar os comentarios
	do{
		frase = strsep(&entrada, "\n");
		linhas++;
		if(frase){

			// Aviso ha algum erro
			if(erros1(frase) == 1){
				printf("ERRO GRAMATICAL: palavra na linha %d!\n", linhas);
				return 1;
			}else if(erros1(frase) == 2){
				printf("ERRO LEXICO: palavra inválida na linha %d!\n", linhas);
				return 1;
			}

				if(frase[0] != '#'){
					frase = ingnoraComentarios(frase);
					separaTermos(frase, linhas);
				}
		}

	}while(frase);

	// Procura por outros erros nao econtrados na erros1
	if(erros2())
		return 1;

	return 0;
}

// Classifica o termo recebido em cada classe de Token
void classifica(char* parametro, int linha){

	Token novo;
	TipoDoToken tipo;
	int i, j;

	i = strlen(parametro);

	if(strcmp(parametro, "LOAD") == 0 || strcmp(parametro, "LOAD-") == 0 || strcmp(parametro, "LOAD|") == 0 ||
	strcmp(parametro, "LOADmq_mx") == 0 || strcmp(parametro, "LOADmq") == 0 || strcmp(parametro, "STOR") == 0 ||
	strcmp(parametro, "JUMP") == 0 || strcmp(parametro, "JMP+") == 0 || strcmp(parametro, "ADD") == 0 ||
	strcmp(parametro, "ADD|") == 0 || strcmp(parametro, "SUB") == 0 || strcmp(parametro, "SUB|") == 0 ||
	strcmp(parametro, "MUL") == 0 || strcmp(parametro, "DIV") == 0 || strcmp(parametro, "LSH") == 0 ||
	strcmp(parametro, "RSH") == 0 || strcmp(parametro, "STORA") == 0)
		tipo = Instrucao;

	else if(parametro[0] == '.')
		tipo = Diretiva;

	else if(parametro[i-1] == ':'){
		if(parametro[0] == '_' || (parametro[0] >= 65 && parametro[0] <= 90) || (parametro[0] >= 97 && parametro[0] <= 122))
			tipo =  DefRotulo;

	}else if(parametro[0] == '"'){
		if(parametro[1] == '0' && parametro[2] == 'x')
			tipo =  Hexadecimal;
		else if(parametro[1] >= 48 && parametro[1] <= 57)
			tipo = Decimal;
		else
			tipo = Nome;

		for(j=0; j<i; j++)
			parametro[j] = parametro[j+1];
		parametro[i-2] = '\0';

	}else if(parametro[0] >= 48 && parametro[0] <= 57){
			if(parametro[1] == 'x')
				tipo =  Hexadecimal;
			else
				tipo = Decimal;

	}else
		tipo = Nome;

	novo.tipo = tipo;
	novo.palavra = parametro;
	novo.linha = linha;

	adicionarToken(novo);
}

// Tudo o que vier depois de um # é considerado comentario e é ignorado
char* ingnoraComentarios(char* frase){

	int i;

	for(i=0; i<strlen(frase); i++){
		if(frase[i] == '#'){
			if(frase[i-1] = ' ')
				frase[i-1] = '\0';
			else
				frase[i] = '\0';

			break;
		}
	}

	return frase;
}

// Procura por alguns tipos de erros lexicos e gramaticais
// Retorna 0 se não ha erros, 1 erros para gramaticais 2 para erros lexicos
int erros1(char* frase){

	int i, j=0, k=0;

	for(i=0; i<strlen(frase); i++){
		// Procura erros relacionados a DefRotulo
		if(frase[i] == ':'){
			j++;
			if(j > 1){
				if(frase[i-1] == ':')
					return 2;
				else
					return 1;
				}
		}

		// Procura erros relacionados a Diretivas
		if(frase[i] == '.'){
			k++;
			if(k > 1){
				if(frase[i-1] == '.')
					return 2;
				else
					return 1;
			}
		}
	}

	return 0;
}

// Procura por outros erros gramaticais nao encontradados na erros1
int erros2(){

	int i, n;
	Token aux;

	for(i=0; i<getNumberOfTokens(); i++){
		aux = recuperaToken(i);

		// Erros em Diretivas
		if(aux.palavra[0] == '.'){

			// Erro na diretiva .set
			if(aux.palavra[1] == 's'){
				aux = recuperaToken(i+1);
				if(aux.tipo != 5){
					printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
					return 1;
				}else{
					aux = recuperaToken(i+2);
					if(aux.tipo != 3 && aux.tipo != 4){
						printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
						return 1;
					}
				}
			// Erro na diretiva .org
			}else if(aux.palavra[1] == 'o'){
				aux = recuperaToken(i+1);
				if(aux.tipo != 3 && aux.tipo != 4){
					printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
					return 1;
				}else if(aux.tipo == 4){
					n = atoi(aux.palavra);
					if(n < 1 || n > 1023){
						printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
						return 1;
					}
				}
			// Erro na diretiva .align
			}else if(aux.palavra[1] == 'a'){
				aux = recuperaToken(i+1);
				if(aux.tipo != 4){
					printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
					return 1;
				}else if(aux.tipo == 4){
					n = atoi(aux.palavra);
					if(n < 1 || n > 1023){
						printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
						return 1;
					}
				}
			// Erro na diretiva .wfill
			}else if(aux.palavra[2] == 'f'){
				aux = recuperaToken(i+1);
				if(aux.tipo != 4){
					printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
					return 1;
				}else if(aux.tipo == 4){
					n = atoi(aux.palavra);
					if(n < 1 || n > 1023){
						printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
						return 1;
					}
				}else{
					aux = recuperaToken(i+2);
					if(aux.tipo != 3 && aux.tipo != 4 && aux.tipo != 5){
						printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
						return 1;
					}
				}
				// Erro na diretiva .word
			}else if(aux.palavra[2] == 'o'){
				aux = recuperaToken(i+1);
				if(aux.tipo != 3 && aux.tipo != 4 && aux.tipo != 5){
					//printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
					return 1;
				}
		}
		// Erro com simbolos
		}else if(aux.tipo == 5){
			if(i > 0){
				aux = recuperaToken(i-1);
				if(aux.palavra[1] != 's' && aux.palavra[1] != 'w' && aux.tipo != 4 && aux.tipo != 0){
					printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
					return 1;
				}
			}else{
				printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
				return 1;
			}
		// Erro nos numeros hexadecimais
		}else if(aux.tipo == 3){
			n = strlen(aux.palavra);
			if(n != 12){
				printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
				return 1;
			}
		// Erro nas Instrucoes
		}else if(aux.tipo == 0 && aux.palavra[2] != 'H'){
			aux = recuperaToken(i+1);
			if(aux.tipo != 3 && aux.tipo != 4 && aux.tipo != 5){
				printf("ERRO GRAMATICAL: palavra na linha %d!\n", aux.linha);
				return 1;
			}
		}
	}
		return 0;
}

// Separa cada linha de comandos em comandos individuais
void separaTermos(char* frase, int linha){

	char* termo;

	do{
		termo = strsep(&frase, " ");
		if(termo){
				if(strlen(termo))
					classifica(termo, linha);
		}
	}while(termo);
}
