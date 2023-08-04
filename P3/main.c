/*
// Projeto SO - exercicio 1, version 03
// Sistemas Operativos, DEI/IST/ULisboa 2017-18
*/


/*Joao Antunes 87668
  Ines Morais 83609 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "matrix2d.h"
#include "argumentos.h"
#include "calculaFatias.h"




/*--------------------------------------------------------------------
| Variaveis Globais 
---------------------------------------------------------------------*/

DoubleMatrix2D     *matrix, *matrixAux;
/*------------------------------------------*/



/*--------------------------------------------------------------------
|fnThread:Cada tarefa calcula o valor da temperatura de cada ponto da sua fatia respetiva e 
|   controla se a maior diferenca observada na matriz e inferior ao limiar de  paragem  maxD 
---------------------------------------------------------------------*/

void* fnThread(void *arg){

    Info *thread = (Info*)arg;
    int n_linhas_fim ,n_linhas_inicio,n;
    double diferencaMaxFatia;                              //diferenca de temperatura maxima para cada fatia
    
    n_linhas_fim = getLinhas(thread)*(getID(thread)+1)+1;   // indica as linhas da matriz onde cada tarefa 
    n_linhas_inicio = getLinhas(thread)*getID(thread)+1;    // deve ler e escrever 
  

    for(n=0; n < getIteracoes(thread); n++){    
      
      diferencaMaxFatia = simul(n_linhas_fim,n_linhas_inicio,getN(thread),getMaxD(thread));
      
      if(esperar_por_todos(getTrabalhadoras(thread),n,getMaxD(thread),diferencaMaxFatia) == 1)  
        return NULL;                                  
    }
  
  return NULL;
}


/*--------------------------------------------------------------------
|parse_integer_or_exit:converte o conteudo em str para int
---------------------------------------------------------------------*/


int parse_integer_or_exit(char const *str, char const *name)
{
  int value;
 
  if(sscanf(str, "%d", &value) != 1) {
    fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name); //canal de erro
    exit(1);
  }
  return value;
}

/*--------------------------------------------------------------------
|parse_double_or_exit:converte o conteudo em str para int
---------------------------------------------------------------------*/

double parse_double_or_exit(char const *str, char const *name)
{
  double value;

  if(sscanf(str, "%lf", &value) != 1) {
    fprintf(stderr, "\nErro no argumento \"%s\".\n\n", name);
    exit(1);
  }
  return value;
}

 

/*--------------------------------------------------------------------
| Function: main
---------------------------------------------------------------------*/

int main (int argc, char** argv) {

  if(argc != 9) {
    fprintf(stderr, "\nNumero invalido de argumentos.\n");
    fprintf(stderr, "Uso: heatSim N tEsq tSup tDir tInf iter trab maxD\n\n");
    return 1;
  }

  /* argv[0] = program name */
  int N = parse_integer_or_exit(argv[1], "N");          // dimencao da matiz
  double tEsq = parse_double_or_exit(argv[2], "tEsq");  //temperaturas das arestas da matriz
  double tSup = parse_double_or_exit(argv[3], "tSup");
  double tDir = parse_double_or_exit(argv[4], "tDir");
  double tInf = parse_double_or_exit(argv[5], "tInf");
  int iter = parse_integer_or_exit(argv[6], "iter");    //numero de iteracoes
  int trab = parse_integer_or_exit(argv[7],"trab");     //numero de tarefas trabalhadoras
  double maxD = parse_double_or_exit(argv[8], "maxD");  // limiar de paragem para a diferenca entre valores de cada iteracao
  int k = N/trab;  //numero de linhas da fatia a ser processada pela tarefa
  int i;


 //Impressao dos argumentos introduzidos 
  fprintf(stderr, "\nArgumentos:\n"
  " N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iter=%d trab=%d maxD=%lf\n",
  N, tEsq, tSup, tDir, tInf, iter, trab,maxD);


  //Validacao dos argumentos introduzidos
  if(N < 1 || tEsq < 0 || tSup < 0 || tDir < 0 || tInf < 0 || iter < 1 || (N % trab) != 0 || trab < 1 || maxD < 0)
  {
    fprintf(stderr, "\nErro: Argumentos invalidos.\n"
  " Lembrar que N ,trabalhadoras e iteracoes>= 1, trabalhadoras e multiplo de N e temperaturas e maxD >= 0 \n\n");
   exit(1);    
  }



  // Inicializacao do mutex e da variavel de condicao
  if(pthread_init() == -1) {
    fprintf(stderr, "\nErro ao inicializar variável de condição e mutex\n");
    return -1;
  }


 
  matrix = dm2dNew(N+2, N+2);      //inicializacao das matrizes

  matrixAux = dm2dNew(N+2, N+2);


  if (matrix == NULL || matrixAux == NULL) {
    fprintf(stderr, "\nErro: Nao foi possivel alocar memoria para matrix ou matrixAux.\n\n");
    return -1;
  }


  dm2dSetLineTo (matrix, 0, tSup);    //temperaturas da arestas de matrix
  dm2dSetLineTo (matrix, N+1, tInf);
  dm2dSetColumnTo (matrix, 0, tEsq);
  dm2dSetColumnTo (matrix, N+1, tDir);

  dm2dCopy(matrixAux, matrix);


  pthread_t tid[trab];  //array de identificadores das tarefas de tamanho trab

  
  Info** args = (Info**) malloc(trab*sizeof(Info*)); //alocacao para ponteiro de estruturas


  for (i = 0; i<trab; i++) {  //Criacao das tarefas
    
    args[i] = InfoNew(N+2,k,iter,trab,i,maxD);  //inicializacao da estrutura com argumentos a serem processados pela tarefa
  
    if (pthread_create (&tid[i], NULL, fnThread,args[i]) != 0){
      perror("Erro ao criar tarefa.\n");
      return 1;
    }
  }


  for (i=0; i<trab; i++) {                      // Ciclo para esperar que todas as tarefas acabem                        
    if (pthread_join (tid[i],NULL) != 0) {
      perror("Erro ao esperar por tarefa.\n");
      return 2;
    }
  }
  

  //destruicao do mutex e variavel de condicao

  if(pthread_destroy() == -1){
    fprintf(stderr, "\nErro ao destruir variável de condição e mutex\n");
    return -1;
  }


  dm2dPrint(matrix);
  
  dm2dFree(matrix);     
  dm2dFree(matrixAux);
  argsFree (args,trab);
  return 0;
}

