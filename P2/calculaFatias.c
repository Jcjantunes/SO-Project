#include "matrix2d.h"
#include "mplib4.h"
#include <stdio.h>
#include <stdlib.h>

/*--------------------------------------------------------------------
|initMatrix:Aloca espaco para uma nova matriz mas neste caso nao e inicializada a 0 o seu conteudo(sendo diferente de dm2dNew)
---------------------------------------------------------------------*/

DoubleMatrix2D* initMatrix(int lines, int columns) {

  DoubleMatrix2D* matrix = malloc(sizeof(DoubleMatrix2D));

  if (matrix == NULL)
    return NULL;

  matrix->n_l = lines;
  matrix->n_c = columns;
  matrix->data = (double*) malloc(sizeof(double)*lines*columns);
  if (matrix->data == NULL) {
    free (matrix);
    return NULL;
 
  }
  return matrix;
}

/*--------------------------------------------------------------------
|trocaMensagens:Realiza a troca de mensagens relativas ao conteudo das fatias entre tarefas  
---------------------------------------------------------------------*/


DoubleMatrix2D *trocaMensagens(DoubleMatrix2D *matrix,int linhas, int colunas, int numIteracoes,int id, int trab){

  /*Entre as duas condicoes o enviarMensagem e o receberMensagens estao alternados para se evitar guardar a mensagem no buffer */

          
  if(id != trab) { /*Condicao onde a ultima tarefa nao entra sendo tambem a unica onde a primeira tarefa entra*/ 

    if(enviarMensagem(id,id+1,dm2dGetLine(matrix,linhas-2),sizeof(double)*(colunas)) == -1){
      fprintf(stderr,"Erro na funcao trocaMensagens\n");
      exit(1);
    }
          
    if(receberMensagem(id+1,id,dm2dGetLine (matrix,linhas-1),sizeof(double)*(colunas)) == -1){
      fprintf(stderr,"Erro na funcao trocaMensagens\n");
      exit(1);
    }
          
    
  }

  if(id != 1) {/*Condicao onde nao entra a primeira tarefa sendo tambem a unica onde a ultima tarefa entra*/ 
            
    if(receberMensagem(id-1,id,dm2dGetLine (matrix,0),sizeof(double)*(colunas)) == -1){
      fprintf(stderr,"Erro na funcao trocaMensagens\n");
      exit(1);
    }


    if(enviarMensagem(id,id-1,dm2dGetLine(matrix,1),sizeof(double)*(colunas)) == -1){
      fprintf(stderr,"Erro na funcao trocaMensagens\n");
      exit(1);
    } 
          
    
        
  }

  /*As tarefas comunicam com a tarefa do o seu identificador-1 e o seu identificador+1 com excecao da primeira e ultima tarefa 
  sendo que apenas comunicam com a do seu identificador+1 e identificador-1 respetivamente*/
      
    
  return matrix;
}

/*--------------------------------------------------------------------
|simul:Realiza o calculo das temperaturas em cada ponto da matriz 
---------------------------------------------------------------------*/

DoubleMatrix2D *simul(DoubleMatrix2D *matrix, DoubleMatrix2D *matrixAux, int linhas, int colunas, int numIteracoes,int id, int trab) {
  int i,j,n;
  double t;

  dm2dCopy(matrixAux, matrix);

  for(n=0;n<numIteracoes;n++){                            //Em cada iteracao e actualizado a temperatura do ponto
    for(i=1;i<linhas-1;i++){                              //Ciclo onde se percorre as linhas da matriz
      for(j=1; j<colunas-1;j++){                          //Ciclo onde se percorre as linhas da matriz
        t=((dm2dGetEntry(matrix,i-1,j)+ 
          dm2dGetEntry(matrix,i+1,j)+dm2dGetEntry(matrix,i,j-1)+dm2dGetEntry(matrix,i,j+1))/4.0);
      dm2dSetEntry(matrixAux,i,j,t);                      //Calculo da temperatura no ponto
      }
    }
    dm2dCopy (matrix,matrixAux);

    /*Condicao onde se apenas existir mais do que uma tarefa trabalhadora terei de trocar mensagens entre as restantes tarefas
    e atualizar o valor da sua fatia*/
    
    if(trab != 1)                                           
      trocaMensagens(matrix,linhas,colunas,numIteracoes,id,trab);
  }

  return matrix;
}



