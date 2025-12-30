all: hw3server hw3client

hw3server: server error_handling
	gcc -g -Wall server error_handling -o hw3server

server: server.c error_handling.h macros.h
	gcc -g -Wall -c server.c -o server

hw3client: client error_handling
	gcc -g -Wall client error_handling -o hw3client

client: client.c error_handling.h macros.h
	gcc -g -Wall -c client.c -o client

error_handling: error_handling.c error_handling.h macros.h
	gcc -g -Wall -c error_handling.c -o error_handling

clean:
	\rm -f hw3server hw3client server client error_handling

