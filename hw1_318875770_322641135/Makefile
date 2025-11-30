all: allocate_handle main 
	gcc main.o allocate_handle.o -o main

main: main.c allocate_handle.h allocate_handle
	gcc -pthread -g -c main.c -o main.o

allocate_handle: allocate_handle.c 
	gcc -pthread -g -c allocate_handle.c -o allocate_handle.o

clean:
	rm main allocate_handle
