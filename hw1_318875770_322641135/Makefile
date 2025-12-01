all: main.o background_processes.o parse_command.o internal_commands.o execute_command.o
	gcc main.o background_processes.o parse_command.o internal_commands.o execute_command.o -o main

main.o: main.c execute_command.h execute_command.c background_processes.h background_processes.c parse_command.h parse_command.c
	gcc -pthread -g -c main.c -o main.o

background_processes.o: background_processes.c
	gcc -pthread -g -c background_processes.c -o background_processes.o

parse_command.o: parse_command.c
	gcc -pthread -g -c parse_command.c -o parse_command.o

internal_commands.o: internal_commands.c
	gcc -pthread -g -c internal_commands.c -o internal_commands.o

execute_command.o: execute_command.c background_processes.c parse_command.c
	gcc -pthread -g -c execute_command.c -o execute_command.o

clean:
	rm  main main.o background_processes.o parse_command.o internal_commands.o execute_command.o
