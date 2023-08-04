# Makefile
# Sistemas Operativos, DEI/IST/ULisboa 2017-18

heatSim: main.o matrix2d.o argumentos.o calculaFatias.o
	gcc -g -o heatSim -pthread main.o matrix2d.o argumentos.o calculaFatias.o

main.o: main.c matrix2d.h argumentos.h
	gcc -Wall -pedantic -std=gnu99 -g -c main.c

matrix2d.o: matrix2d.c matrix2d.h
	gcc -g -c matrix2d.c


argumentos.o: argumentos.c argumentos.h
	gcc -g -c argumentos.c

calculaFatias.o: calculaFatias.c calculaFatias.h 

clean:
	rm -f *.o heatSim



