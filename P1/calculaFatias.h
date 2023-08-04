#ifndef CALCULAFATIAS_H
#define CALCULAFATIAS_H

DoubleMatrix2D* initMatrix(int lines, int columns);
DoubleMatrix2D *trocaMensagens(DoubleMatrix2D *matrix, int linhas, int colunas, int numIteracoes,int id, int trab);
DoubleMatrix2D *simul(DoubleMatrix2D *matrix, DoubleMatrix2D *matrixAux, int linhas, int colunas, int numIteracoes,int id, int trab);

#define getData(m) m->data

#endif