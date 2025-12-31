#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include "error_handling.h"
#include "macros.h"


static int create_socket(void) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            print_sys_call_error("socket");
            exit(EXIT_FAILURE);
        }
        return sockfd;
}

static void connect_socket(int sockfd, char *server_addr, int server_port) {
    // Declare IPv4 address
    struct hostent *server;
    struct sockaddr_in serv_addr;
    // Resolve hostname or IP address
    server = gethostbyname(server_addr);
    if (server == NULL) {
        print_error("ERROR, no such host");
        exit(EXIT_FAILURE);
    }
    // Zero out to avoid garbage values
    memset(&serv_addr, 0, sizeof(serv_addr));
    // Configure address structure
    serv_addr.sin_family   = AF_INET;        // IPv4 only
    serv_addr.sin_port = htons(server_port);
    // Copy Resulved address into serv_addr
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    // Connect to server (initiates TCP 3-way handshake)
    if (connect(sockfd, (struct sockaddr *)(&serv_addr), sizeof(serv_addr)) < 0) {
        print_sys_call_error("connect");
        exit(EXIT_FAILURE);
    }
}

static void notify_name_to_server(int sockfd, char* client_name) {
    // Implementation to notify the server of the client's name
    ssize_t send_name = send(sockfd, client_name, strlen(client_name), MSG_NOSIGNAL);
    if (send_name < 0) {
        print_sys_call_error("send");
        exit(EXIT_FAILURE);
    }
}

int main (int argc, char *argv[]) {
    if (argc != 4) {
        print_error("Invalid number of arguments: hw3client addr port name");
        exit(EXIT_FAILURE);
    }
    // Extract arguments:
    char* server_addr = argv[1];
    int server_port = atoi(argv[2]);
    char* client_name = argv[3];

    // Check for port validity
    if (server_port <= MIN_PORT_NUM || server_port > MAX_PORT_NUM) {
        print_error("Invalid port number");
        exit(EXIT_FAILURE);
    }
    // Create new socket
    int sockfd = create_socket();
    connect_socket(sockfd, server_addr, server_port);
    // Notify client name to server - after establishing a good connection
    notify_name_to_server(sockfd, client_name);

    // Variables for the main client loop
    fd_set read_fds;
    int max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;
    char buffer[MAX_LEN_USER_MSG + 1];
    int sent_exit = 0; // Flag to indicate if exit command was sent
    // Main client loop (select loop - allows to handle input and server messages in parallel)
    while (1)
    {
        // Setup fd set (must reset every iteration)
        FD_ZERO(&read_fds);               // Clear the set
        FD_SET(STDIN_FILENO, &read_fds);  // Add stdin to set
        FD_SET(sockfd, &read_fds);        // Add socket to set
        // Wait for activity using select()
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            print_sys_call_error("select");
        }
        // Handle Server Message
        if (FD_ISSET(sockfd, &read_fds)) {
            // Handle server message
            ssize_t server_msg = read(sockfd, buffer, MAX_LEN_USER_MSG);
            if (server_msg < 0) {
                print_sys_call_error("read");
                exit(EXIT_FAILURE);
            }
            if (server_msg == 0) {
                // Server has closed the connection unexpectedly (its not supposed to happen according to the requirements)
               print_error("Server disconnected unexpectedly");
               exit(EXIT_FAILURE);
            }
            // Print server message
            buffer[server_msg] = '\0'; // Null-terminate the received message
            printf("%s", buffer);

        }
        if (sent_exit) {
            // Exit command was sent, break the loop
            printf("client exiting\n");
            break;
        }
        // Handle User Input
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                // EOF reached (Ctrl+D pressed)
                // Close connection gracefully
                print_error("Didn't get the user message, exiting");
                break;
            }
            // send message to the server
            ssize_t user_msg = send(sockfd, buffer, strlen(buffer), MSG_NOSIGNAL);
            if (user_msg < 0) {
                print_sys_call_error("send");
                exit(EXIT_FAILURE);
            }
            // Check for exit command
            if (strncmp(buffer, "!exit", 5) == 0) {
                sent_exit = 1;
            }
        }
    }
    close(sockfd);
    return 0;
}
