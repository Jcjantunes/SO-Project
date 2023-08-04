# Makefile
# Sistemas Operativos, DEI/IST/ULisboa 2017-18

heatSim: main.o matrix2d.o mplib4.o leQueue.o argumentos.o calculaFatias.o
	gcc -g -o heatSim -pthread main.o matrix2d.o mplib4.o leQueue.o argumentos.o calculaFatias.o

main.o: main.c matrix2d.h mplib4.h argumentos.h
	gcc -Wall -pedantic -std=gnu99 -g -c main.c

matrix2d.o: matrix2d.c matrix2d.h
	gcc -g -c matrix2d.c

mplib4.o: mplib4.c mplib4.h
	gcc -g -c mplib4.c

leQueue.o: leQueue.c leQueue.h
	gcc -g -c leQueue.c

argumentos.o: argumentos.c argumentos.h
	gcc -g -c argumentos.c

calculaFatias.o: calculaFatias.c calculaFatias.h 

clean:
	rm -f *.o heatSim



