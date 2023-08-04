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
#include "mplib3.h"
#include "argumentos.h"
#include "calculaFatias.h"


  //-------------------------------------------

/*--------------------------------------------------------------------
|fnThread:recebe as fatias da main e calcula o valor da temperatura dos seus pontos
---------------------------------------------------------------------*/

void* fnThread(void *arg){

    Info *thread = (Info*)arg;

    DoubleMatrix2D *matrixT, *matrixTaux;
    

    matrixT = initMatrix(getLinhas(thread),getN(thread));

    matrixTaux = initMatrix(getLinhas(thread),getN(thread));

    if(matrixT == NULL || matrixTaux == NULL){
      fprintf(stderr, "Erro ao alocar matrixT ou matrixTaux em fnThread\n");
      exit(1);
    }


    
    /*Recebe da main a fatia corresponde da tarefa*/ 
    if(receberMensagem(0,getID(thread),getData(matrixT) ,(sizeof(double)*(getLinhas(thread))*(getN(thread)))) == -1){ 
      fprintf(stderr,"Erro a receber mensagem em fnThread\n");
      exit(1); 
    }
    
    simul(matrixT,matrixTaux,(getLinhas(thread)),getN(thread),getIteracoes(thread),getID(thread),getTrabalhadoras(thread));


    /*Envia de volta para a main a fatia correspondente da tarefa com a temperatura dos seus pontos calculados*/
    if(enviarMensagem(getID(thread),0,getData(matrixT) ,(sizeof(double)*(getLinhas(thread))*(getN(thread)))) == -1){
      fprintf(stderr,"Erro a enviar mensagem em fnThread\n");
      exit(1);
    }

    dm2dFree(matrixT);
    dm2dFree(matrixTaux);
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
    fprintf(stderr, "Uso: heatSim N tEsq tSup tDir tInf iter trab csz\n\n");
    return 1;
  }

  /* argv[0] = program name */
  int N = parse_integer_or_exit(argv[1], "N");  // dimencao da matiz
  double tEsq = parse_double_or_exit(argv[2], "tEsq"); //temperaturas das arestas da matriz
  double tSup = parse_double_or_exit(argv[3], "tSup");
  double tDir = parse_double_or_exit(argv[4], "tDir");
  double tInf = parse_double_or_exit(argv[5], "tInf");
  int iter = parse_integer_or_exit(argv[6], "iter"); //numero de iteracoes
  int trab = parse_integer_or_exit(argv[7],"trab");  //numero de tarefas trabalhadoras
  int csz = parse_integer_or_exit(argv[8],"csz"); //numero de mensagens por canal
  int k = N/trab;  //numero de linhas da fatia a ser processada pela tarefa
  int i,j,x;


  fprintf(stderr, "\nArgumentos:\n"
  " N=%d tEsq=%.1f tSup=%.1f tDir=%.1f tInf=%.1f iter=%d trab=%d csz=%d\n",
  N, tEsq, tSup, tDir, tInf, iter, trab, csz);


  //Validacao dos argumentos introduzidos
  if(N < 1 || tEsq < 0 || tSup < 0 || tDir < 0 || tInf < 0 || iter < 1 || (N % trab) != 0 || trab < 1 || csz < 0)
  {
  	fprintf(stderr, "\nErro: Argumentos invalidos.\n"
  " Lembrar que N ,trabalhadoras e iteracoes>= 1 e temperaturas e csz >= 0 e csz >= 1 \n\n");
	exit(1);    
  }



  if (inicializarMPlib(csz,trab+1) == -1) {  //inicializacao do canal de mensagens
    fprintf(stderr,"Erro ao inicializar MPLib.\n"); 
    return 1;
  }

  DoubleMatrix2D *matrix,*result,*fatia;
  
  matrix = dm2dNew(N+2, N+2);

  if (matrix == NULL) {
    fprintf(stderr, "\nErro: Nao foi possivel alocar memoria para matrix.\n\n");
    return -1;
  }

  dm2dSetLineTo (matrix, 0, tSup);    //temperaturas da arestas de matrix
  dm2dSetLineTo (matrix, N+1, tInf);
  dm2dSetColumnTo (matrix, 0, tEsq);
  dm2dSetColumnTo (matrix, N+1, tDir);

  fatia = dm2dNew(k+2, N+2);

  if (fatia == NULL) {
    fprintf(stderr, "\nErro: Nao foi possivel alocar memoria para fatia.\n\n");
    return -1;
  }

  result = initMatrix(k+2,N+2); 

  if (result == NULL) {
    fprintf(stderr, "\nErro: Nao foi possivel alocar memoria para result.\n\n");
    return -1;
  }


  pthread_t tid[trab];  //array de identificadores das tarefas de tamanho trab

  
  Info** args = (Info**) malloc(trab*sizeof(Info*)); //alocacao para ponteiro de estruturas




  for (i = 0; i < trab; i++) {  //Criacao das tarefas

    
    args[i] = InfoNew(N+2,k+2,iter,trab,i+1);  //inicializacao da estrutura com argumentos a serem processados pela tarefa
  
    if (pthread_create (&tid[i], NULL, fnThread,args[i]) != 0){
      perror("Erro ao criar tarefa.\n");
      return 1;
    }


    if(trab != 1) {  //condicao onde existe mais do que uma tarefa trabalhadora sendo necessario separar a matriz por fatias

      x=0;

      for(j = (k*i); j <= ((k*(i+1))+1); j++)          //separacao da matriz por fatias            
        dm2dSetLine (fatia,x++,dm2dGetLine(matrix,j));
      
      if (enviarMensagem(0,i+1,getData(fatia) , sizeof(double)*(k+2)*(N+2)) == -1){  //envio da fatia da tarefa correspondente para fnthread
        fprintf(stderr,"Erro a enviar mensagem na main\n");
        exit(1);             
      }
    }
    

    else  //nao existe separacao da matrix por fatias
      if(enviarMensagem(0,i+1,getData(matrix), sizeof(double)*(k+2)*(N+2)) == -1){ //envio da matrix para fnthread 
        fprintf(stderr,"Erro a enviar mensagem na main\n");
        exit(1);
      }

  }
 
  dm2dFree (fatia);



  for (i=0; i<trab; i++) {
    /* recebe de fnThread a fatia correspondente a tarefa com a temperatura dos seus pontos calculada*/ 

     if(receberMensagem(i+1,0,getData(result),sizeof(double)*(k+2)*(N+2)) == -1){ 
      fprintf(stderr,"Erro a receber mensagem na main\n");
      exit(1);
    }


    if (pthread_join (tid[i],NULL) != 0) {
      perror("Erro ao esperar por tarefa.\n");
      return 2;
    }

    if(result == NULL){
      fprintf(stderr,"Erro ao devolver result na main\n");
      exit(1);         
    }

    if(trab == 1){ //caso exista apenas uma tarefa trabalhadora nao sera necessario guardar o conteudo de result em matrix
      dm2dPrint(result);
    }

    if(trab != 1) {

      x=1;
      //actualizacao do conteudo de matrix pelo conteudo da fatia nas linhas correspondentes da fatia
      for(j=(k*i)+1; j < ((k*(i+1))+1) ;j++)
        dm2dSetLine (matrix,j,dm2dGetLine(result,x++));

    }
    
  
  }

  if(trab != 1)
    dm2dPrint(matrix);

  dm2dFree(result);
  dm2dFree(matrix);
  libertarMPlib();
  argsFree (args,trab);
  return 0;
}

