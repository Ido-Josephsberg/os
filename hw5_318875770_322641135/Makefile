all: hw5

hw5: hw5.o
	gcc -o hw5 hw5.o

hw5.o: hw5.c xv6fs.h
	gcc -c hw5.c -o hw5.o

clean:
	rm -f hw5.o hw5
