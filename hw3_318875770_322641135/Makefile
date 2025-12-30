all: server client error_handling
	gcc -g -Wall  server client error_handling -o hw3

server: server.c error_handling.h
	gcc -g -Wall -c server.c -o hw3server

client: client.c error_handling.h
	gcc -g -Wall -c client.c -o hw3client

error_handling: error_handling.c error_handling.h
	gcc -g -Wall -c error_handling.c -o error_handling

clean:
	\rm hw3 server disclientpatcher error_handling
