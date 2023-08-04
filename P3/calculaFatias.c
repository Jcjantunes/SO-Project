#include "matrix2d.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

extern DoubleMatrix2D *matrix, *matrixAux;

pthread_mutex_t    mutex_espera;        
pthread_cond_t     esperaIteracao;


/*--------------------------------------------------------------------
|simul:Realiza o calculo das temperaturas em cada ponto da matriz e da diferenca
|     de temperturas de cada ponto entre iteracoes 
---------------------------------------------------------------------*/

double simul(int linhas_fim, int linhas_inicio, int colunas, double maxD) {
  int i,j;
  double t, diferencaMaxFatia=0, diferencaPonto;


    for(i=linhas_inicio; i<linhas_fim; i++){                   //Ciclo onde se percorre as linhas da matriz
      for(j=1; j<colunas-1;j++){                               //Ciclo onde se percorre as colunas da matriz
        t=((dm2dGetEntry(matrix,i-1,j)+ 
          dm2dGetEntry(matrix,i+1,j)+dm2dGetEntry(matrix,i,j-1)+dm2dGetEntry(matrix,i,j+1))/4.0); //Calculo da temperatura de cada ponto
        
        if(maxD != 0){
          diferencaPonto = t-dm2dGetEntry(matrix,i,j);   //calculo da diferenca de tempertura em cada ponto da matriz entre iteracoes 

        
          if(diferencaPonto > diferencaMaxFatia)         //calculo da diferenca de temperatura maxima para cada fatia
            diferencaMaxFatia = diferencaPonto;
        }

        dm2dSetEntry(matrixAux,i,j,t);                      
      }
    }

  return diferencaMaxFatia;
}



/*--------------------------------------------------------------------
| esperar_por_todos: funcao que funciona como barreira sincronizando as tarefas que 
|  impede que uma tarefa comece a proxima itercao sem outra ter acabado a mesma
---------------------------------------------------------------------*/


int esperar_por_todos(int trabalhadoras, int iteracao,double maxD,double diferencaMaxFatia){

  static int numTrabalhadoras = 0;           // contador de tarefas trabalhadoras que entram na funcao
  static int numIteracoes = 0;               // contador de iteracoes
  static double diferencaMaxMatrix = 0;      // diferenca maxima de temperatura observada na matriz 
  static int flag = 0;                       // indica se o programa de terminar ou continuar  
  DoubleMatrix2D *tmp;                         
  


  if(pthread_mutex_lock(&mutex_espera) != 0) {
    fprintf(stderr, "\nErro ao bloquear mutex\n");
    return -1;
  }


  if(maxD!=0){                                   // calcula entre tarefas qual e a maior diferenca de 
    if(diferencaMaxFatia > diferencaMaxMatrix)   // temperatura na matriz
      diferencaMaxMatrix = diferencaMaxFatia;
  }

  if(++numTrabalhadoras == trabalhadoras){                // apenas a ultima tarefa pode libertar as tarefas anteriores isto acontece
    if(pthread_cond_broadcast(&esperaIteracao) != 0) {     // quando o contador de trabalhadoras for igual ao numero total de trabalhadoras
      fprintf(stderr, "\nErro ao por a tarefa em espera\n");
      return -1;
    }

    numTrabalhadoras = 0;                        
    numIteracoes++;                              // incrementa o contador de iteracoes que impede que a ultima tarefa entre em espera e 
                                                 // permite que as outras tarefas saiam do ciclo de espera ativa ao serem libertadas

    
    if(maxD != 0){                             
                                               // condicao para verificar se a maior diferenca de tempertura
      if(diferencaMaxMatrix < maxD)            // da matriz e inferior ao limiar de paragem
        flag = 1;                              // modifica variavel de retorno de acordo com o resultado 
      else                                     // indicando se o programa deve terminar ou prosseguir
        diferencaMaxMatrix=0;                       
    }


    tmp = matrixAux;
    matrixAux = matrix;
    matrix = tmp; 
    
  }
  

  while(numIteracoes == iteracao){                       // tarefas ficam em espera ate que a ultima tarefa as liberte e assinale
    if(pthread_cond_wait(&esperaIteracao, &mutex_espera) != 0){    // que e possivel passar para a proxima iterecao
      fprintf(stderr, "\nErro ao por a tarefa em espera\n");
      return -1;
    }   
  }
  

  if(pthread_mutex_unlock(&mutex_espera) != 0) {
    fprintf(stderr, "\nErro ao desbloquear mutex\n");
    return -1;
  }
  

  return flag;
}





/*--------------------------------------------------------------------
| pthread_init : inicializa o mutex e a varivel de condicao 
---------------------------------------------------------------------*/


int pthread_init(){
  
  if(pthread_mutex_init(&mutex_espera, NULL) != 0) {       
    fprintf(stderr, "\nErro ao inicializar mutex\n");
    return -1;
  }


  if(pthread_cond_init(&esperaIteracao, NULL) != 0) {
    fprintf(stderr, "\nErro ao inicializar variável de condição\n");
    return -1;
  }

  return 0;
}



/*--------------------------------------------------------------------
| pthread_init : destroi o mutex e a varivel de condicao
---------------------------------------------------------------------*/

int pthread_destroy(){

   if(pthread_mutex_destroy(&mutex_espera) != 0) {
    fprintf(stderr, "\nErro ao destruir mutex\n");
    return -1;
  }

  if(pthread_cond_destroy(&esperaIteracao) != 0) {
    fprintf(stderr, "\nErro ao destruir variável de condição\n");
    return -1;
  }

  return 0;
}


