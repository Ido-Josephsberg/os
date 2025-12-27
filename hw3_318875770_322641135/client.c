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

// TODO: Make sure you treat the NOTES in the code
// TODO: Make sure what do to if server sends am empty message 

static int create_socket(void) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            sys_call_error("socket");
            //NOTE: Currently exit(EXIT_FAILURE) inside sys_call_error()
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
        error("ERROR, no such host");
    }
    // Zero out to avoid garbage values
    memset(&serv_addr, 0, sizeof(serv_addr));
    // Configure address structure
    serv_addr.sin_family   = AF_INET;        // IPv4 only
    serv_addr.sin_port = htons(server_port);
    // Copy Resulved address into serv_addr
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    // Connect to server (initiates TCP 3-way handshake)
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        sys_call_error("connect");
        //NOTE:Currently exit(EXIT_FAILURE) inside sys_call_error()
    }
}

static void notify_name_to_server(int sockfd, char* client_name) {
    // Implementation to notify the server of the client's name
    char name_to_notify[MAX_LEN_USER_MSG + 1]; // MAX_LEN_USER_MSG 256 not including Null terminator
    snprintf(name_to_notify, sizeof(name_to_notify), "Client Name: %s", client_name);
    // Send name to the server
    ssize_t send_name = write(sockfd, name_to_notify, strlen(name_to_notify));
    if (send_name < 0) {
        sys_call_error("write");
        //NOTE:Currently exit(EXIT_FAILURE) inside sys_call_error()
    }
}

int main (int argc, char *argv[]) {
    if (argc != 4) {
        error("Invalid number of arguments: hw3client addr port name");
        //NOTE:Currently exit(EXIT_FAILURE) inside error()
    }
    // Extract arguments:
    char* server_addr = argv[1];
    int server_port = atoi(argv[2]);
    char* client_name = argv[3];

    // Check for port validity
    if (server_port <= 0 || server_port > 65535) {
        error("Invalid port number");
    }
    // Create new socket
    int sockfd = create_socket();
    connect_socket(sockfd, server_addr, server_port);
    // Notify client name to server - after establishing a good connection
    notify_name_to_server(sockfd, client_name);
    
    // Variables for the main client loop
    fd_set read_fds;
    int max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;
    char buffer[MAX_LEN_USER_MSG + 1]; // TODO: check this
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
            sys_call_error("select");
        }
        // Handle Server Message
        if (FD_ISSET(sockfd, &read_fds)) {
            // Handle server message
            ssize_t server_msg = read(sockfd, buffer, MAX_LEN_USER_MSG);
            if (server_msg < 0) {
                sys_call_error("read");
                //NOTE: Currently exit(EXIT_FAILURE) inside sys_call_error()
            }
            if (server_msg == 0) {
                // Can the server close connection?
                /* TODO: If server can not break connection remove this if (server_msg == 0) {...}
                */
               printf("DEBUG: Server Disconnected\n");
               break;
               // TODO: if it cant be empty than remove until here. 
            }
            // Print server message
            buffer[server_msg] = '\0'; // Null-terminate the received message
            printf("%s", buffer);
            fflush(stdout); // NOTE: not sure if needed. // Force immediate display
        }
        // Handle User Input
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                // EOF reached (Ctrl+D pressed)
                // Close connection gracefully
                break;
            }
            // send message to the server
            ssize_t user_msg = write(sockfd, buffer, strlen(buffer));
            if (user_msg < 0) {
                sys_call_error("write");
                //NOTE:Currently exit(EXIT_FAILURE) inside sys_call_error()
            }
            // TODO: make sure here that the messege is being sent with '\n'
            // DEBUG PART:
            // Check if message ends with newline (fgets includes \n)
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                printf("DEBUG: user msg is sent to server with newline\n");
            }
            // END DEBUG PART

            // Check for exit command
            if (strncmp(buffer, "!exit", 5) == 0) {
                printf("client exiting\n");
                break;
            }
        }
    }
    close(sockfd);
    return 0;
}