#ifndef ARGUMENTOS
#define ARGUMENTOS


typedef struct info {
  int id;
  int N;
  int n_linhas;
  int iter;
  int trab;
  double maxD;
} Info;

Info* InfoNew(int dimensao,int num_linhas,int iteracoes,int trabalhadoras,int indice,double diferenca);
void argsFree (Info** args,int trab);

#define getN(m)             m->N
#define getLinhas(m)        m->n_linhas
#define getIteracoes(m)     m->iter
#define getTrabalhadoras(m) m->trab
#define getID(m)            m->id
#define getMaxD(m)          m->maxD
#define getMaximoFatia(m)   m->maximo_fatia

#endif