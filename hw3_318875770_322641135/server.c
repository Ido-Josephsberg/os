#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "macros.h"
#include "error_handling.h"
#define MAX_EVENTS 16

static int init_socket(int port) {
    // Initialize server socket
    int sock_fd; // File descriptor for the socket
    

    // Creating socket file descriptor
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        // Notify about the error and exit
        print_sys_call_error("socket");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    // Set socket options to allow reuse of address and port
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        print_sys_call_error("setsockopt");
        exit(EXIT_FAILURE);
    }
    return sock_fd;
}

static void init_server_socket(int port, struct sockaddr_in *server_addr, struct sockaddr_in *client_addr) {
    // Set socket options
    // Clear the server_addr structure
    bzero(server_addr, sizeof(*server_addr));
    server_addr->sin_family = AF_INET; // IPv4
    server_addr->sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any address
    server_addr->sin_port = htons(port); // Convert port number to network byte order
}

static void bind_and_listen(int sock_fd, struct sockaddr_in *server_addr) {
    // Bind the socket to the specified port
    if (bind(sock_fd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        // Notify about the error and exit
        print_sys_call_error("bind");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    listen(sock_fd, MAX_EVENTS);
}

static int create_epoll_instance() {
    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        // Notify about the error and exit
        print_sys_call_error("epoll_create1");
        exit(EXIT_FAILURE);
    }
    return epoll_fd;
}

static int wait_for_events(int epoll_fd, struct epoll_event *events) {
    // Wait for events on the epoll instance and return the number of events.
    // Wait for events and save the number of file descriptors ready for reading.
    int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    // Check for errors
    if (num_events == -1) {
        // Notify about the error and exit
        print_sys_call_error("epoll_wait");
        exit(EXIT_FAILURE);
    }

    return num_events;
}

static int add_client_connection(int socket_fd, struct sockaddr_in *client_addr, client_info *clients, struct epoll_event *clients_event, int epoll_fd, int *curr_client_count) {
    // Accept new client connection and add it to clients array and epoll instance
    // Client address length
    socklen_t client_len = sizeof(*client_addr);
    // Accept new client connection
    int new_sock_fd = accept(socket_fd, (struct sockaddr *)client_addr, &client_len);
    if (new_sock_fd == -1) {
        // Notify about the error and continue to the next iteration
        print_sys_call_error("accept");
        return 1;
    }
    // Add new client to clients array
    clients[*curr_client_count].fd = new_sock_fd;
    clients[*curr_client_count].addr = client_addr->sin_addr;
    clients[*curr_client_count].has_name = 0; // Initially, client has no name
    (*curr_client_count)++;
    // TODO: CONSIDER change fd to non-blocking mode
    // Add new socket to epoll instance
    clients_event->events = EPOLLIN; // Monitor for input events
    clients_event->data.fd = new_sock_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_sock_fd, clients_event);
    return 0;
}

static client_info* get_client_by_fd(int fd, client_info *clients, int curr_client_count) {
    // Return pointer to client_info struct for the given file descriptor
    // Assume fd is valid and exists in clients array
    for (int i = 0; i < curr_client_count; i++)
    {
        if (clients[i].fd == fd) {
            return (clients + i);
        }
    }
    return NULL; // Should not reach here if fd is valid
}

static void announce_new_client(client_info *new_client) {
    // Display on server console that a new client has connected
    // Read the client's name from the client into client_info struct. Notify and return if error occurs.
    int n = recv(new_client->fd, new_client->name, MAX_LEN_USER_MSG, 0);
    if (n == -1) {
        print_sys_call_error("recv");
        return;
    }
    // Null-terminate the received name
    (new_client->name)[n] = '\0';
    // Mark that the client has a name
    new_client->has_name = 1;
    // Get client IP address as string
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(new_client->addr), client_ip, INET_ADDRSTRLEN);
    // Print client connection message
    printf("client %s connected from %s\n", new_client->name, client_ip);
}

static void send_message_to_client(client_info *sender, int client_fd, char *message) {
    // Send message to the client with the given file descriptor. Add sender's name as prefix.
    char msg_to_send[MAX_LEN_USER_MSG + 3];
    sprintf(msg_to_send, "%s: %s", sender->name, message); // Prefix for the message
    // Send message to the client with the given file descriptor.
    if (send(client_fd, msg_to_send, strlen(msg_to_send) + 1, MSG_NOSIGNAL) == -1) {
        print_sys_call_error("send");
    }
}

static int search_client_by_name(char *name, client_info *clients, int curr_client_count) {
    // Search for a client by name in the clients array
    for (int i = 0; i < curr_client_count; i++) {
        if (clients[i].has_name && strcmp(clients[i].name, name) == 0) {
            return clients[i].fd; // Return the file descriptor if found
        }
    }
    return -1; // Return -1 if not found
}

static void whisper_message(client_info *sender, char *message, client_info *clients, int curr_client_count) {
    // Handle whisper message from sender.
    // Extract recipient name and actual message
    char recipient_name[MAX_LEN_USER_MSG];
    // Save recipient name
    strcpy(recipient_name, message + 1); // Skip '@' character
    recipient_name[strcspn(recipient_name, " ")] = 0; // Terminate at first space
    // Find recipient file descriptor
    int recipient_fd = search_client_by_name(recipient_name, clients, curr_client_count);
    // If recipient not found, ignore the whisper
    if (recipient_fd == -1)
        return;
    // Send the whisper message to the recipient
    send_message_to_client(sender, recipient_fd, message);
}

static void normal_message(client_info *sender, char *message, client_info *clients, int curr_client_count) {
    // Send message to all connected clients
    for (int i = 0; i < curr_client_count; i++) {
        // Send message to the client
        send_message_to_client(sender, clients[i].fd, message);
    }
}

static void exit_client(client_info *sender, client_info *clients, int *curr_client_count, int epoll_fd) {
    // Handle client exit
    // Remove sender file descriptor from epoll instance and notify about error if occurs
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sender->fd, NULL) == -1)
        print_sys_call_error("epoll_ctl");
    // Close sender's socket
    close(sender->fd);
    // Notify about client disconnection
    printf("client %s disconnected\n", sender->name);
    // Remove sender from clients array
    if (*curr_client_count > 1)
        // replace sender with the last client in the array
        *sender = clients[*curr_client_count - 1];
    // Decrease current client count
    (*curr_client_count)--;
    
}

static void exit_message(client_info *sender, client_info *clients, int *curr_client_count, int epoll_fd) {
    // Handle client exit
    // Normall message to all clients
    normal_message(sender, "!exit", clients, *curr_client_count);
    //shutdown(sender->fd, SHUT_RD);
    exit_client(sender, clients, curr_client_count, epoll_fd);
}

static void send_client_message(client_info *curr_client, client_info *clients, int *curr_client_count, int epoll_fd) {
    // Receive message from client and send client's message.
    // Receive message from client. Notify and return if error occurs.
    char msg_buffer[MAX_LEN_USER_MSG + 1];
    // Receive message from client
    int n = recv(curr_client->fd, msg_buffer, MAX_LEN_USER_MSG, 0);
    if (n == -1) {
        print_sys_call_error("recv");
        return;
    }
    msg_buffer[n] = '\0'; // Null-terminate the received message
    if (n == 0)
        // Client disconnected
        exit_client(curr_client, clients, curr_client_count, epoll_fd);

    // Check whether it normal, whisper or exit message
    else if (msg_buffer[0] == '@') {
        // Send whisper message
        whisper_message(curr_client, msg_buffer, clients, *curr_client_count);
    }
    else if (strcmp(msg_buffer, "!exit") == 0) {
        // Handle exit message
        exit_message(curr_client, clients, curr_client_count, epoll_fd);
    }
    else {
        // Send normal message
        normal_message(curr_client, msg_buffer, clients, *curr_client_count);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        // Print usage information and exit
        print_error("Usage: hw3server <port>\n");
        exit(EXIT_FAILURE);
    }
    int curr_client_count = 0; // Current number of connected clients
    struct sockaddr_in server_addr, client_addr; // Server and client address structure
    struct epoll_event main_socket_event, events[MAX_EVENTS]; // Epoll events structures of clients inputs/ connections requests.
    client_info clients[MAX_EVENTS]; // Array to store client information
    // Convert port argument to integer
    int port = atoi(argv[1]);
    // Initialize the server socket
    int socket_fd = init_socket(port);
    // Initialize server address structure
    init_server_socket(port, &server_addr, &client_addr);
    // Bind and listen on the server socket
    bind_and_listen(socket_fd, &server_addr);
    // Create epoll instance
    int epoll_fd = create_epoll_instance();
    // Configure main socket event
    main_socket_event.events = EPOLLIN; // Monitor for input events
    main_socket_event.data.fd = socket_fd;
    // Add server socket to epoll instance to monitor incoming connections
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &main_socket_event);
    // Handle incoming connections and events
    while (1)
    {
        int num_events = wait_for_events(epoll_fd, events);
        for (int i = 0; i < num_events; i++) {
            // New client connection
            if (events[i].data.fd == socket_fd) {
                // Accept new client connection
                if (add_client_connection(socket_fd, &client_addr, clients, (events + i), epoll_fd, &curr_client_count) != 0)
                    continue; // Error occurred while adding client connection, skip to next event
            }
            else {
                // Handle client data
                int client_fd = events[i].data.fd;
                client_info *curr_client = get_client_by_fd(client_fd, clients, curr_client_count);
                if(curr_client == NULL) { 
                    // Invalid client fd, skip to next event
                    continue;
                }
                if (curr_client->has_name == 0) {
                    // Announce new client
                    announce_new_client(curr_client);
                }
                else {
                    // Receive message from client and send client's message.
                    send_client_message(curr_client, clients, &curr_client_count, epoll_fd);
                }
            }
        }
        
    }
    



    return 0;
}