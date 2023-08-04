#include "argumentos.h"
#include <stdio.h>
#include <stdlib.h>



/*Inicializacao da estrutura onde se ira introduzir os "argumentos" que fnthread tera de aceder*/

Info* InfoNew(int dimensao,int num_linhas,int iteracoes,int trabalhadoras,int indice){
  Info* args = malloc(sizeof(Info));

  if (args == NULL)
    return NULL;

  args->N = dimensao;          //dimencao da matriz
  args->n_linhas = num_linhas; //numero de linhas da fatia
  args->iter = iteracoes;      //numero de iteracoes 
  args->id = indice;           //identificador da tarefa
  args->trab = trabalhadoras;  //numero de tarefas trabalhadoras
  return args;
}


void argsFree (Info** args, int trab) { 
	int i;
   	for (i = 0; i < trab; i++) //libertacao de todas as estruturas correspondentes as tarefas
    	free(args[i]);
	free (args);  //libertacao do ponteiro para estruturas
}