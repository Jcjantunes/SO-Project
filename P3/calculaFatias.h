#ifndef CALCULAFATIAS_H
#define CALCULAFATIAS_H

double simul(int linhas_fim, int linhas_inicio, int colunas, double max);
int esperar_por_todos(int trabalhadoras, int iteracao,double max_d,double difMax);
int pthread_init();
int pthread_destroy();

#endif