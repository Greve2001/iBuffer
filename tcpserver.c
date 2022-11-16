#include "common.h"

int own_socket;
int last_client_socket; // Fix

/*
* Start tcp server
* - Create socket
* @param ip is the Internet host address
*/
void start_tcp_server(char * ip) 
{
    // create endpoint for communication and return a file descriptor that refers to that endpoint.
    // * AF_INET is IPv4. SOCK_STREAM provides TCP. Default protocol is 0 when only a single protocol is used.
    own_socket = socket(AF_INET, SOCK_STREAM, 0); // filedescriptor
    if (own_socket < 0) 
    {
        updateWindow("Socket creation failed...");
        exit(1);
    }

    // struct for handling internet addresses defined in <netinit/in.h>. 
    struct sockaddr_in server_addr;

    // set port and address family
    server_addr.sin_port = htons(1504);
    server_addr.sin_family = AF_INET; // TCP
    
    // converts from the IPv4 numbers-and-dots notation into binary form
    inet_aton(ip, &server_addr.sin_addr); 
    
    socklen_t addr_len = sizeof(server_addr);  

    // assign address to socket
    if (bind(own_socket, (struct sockaddr*) &server_addr, addr_len) < 0) 
    {
        updateWindow("Binding to port failed...");
        exit(1);
    }

    // Listen for clients
    // * if the queue is full (max 5) the client trying to connect will receive ECONNREFUSED    
    if (listen(own_socket, 5) < 0) 
        updateWindow("Listening failed...");

    struct sockaddr_in client_addr;
    
    // Extract connection and create a new connected socket and handle this in a new thread
    for(;;)
    {
        // extract the first connection from the listening socket.
        // create a new connected socket, and return new file descriptor referring to this socket
        int client_socket = accept(own_socket, (struct sockaddr*) &client_addr, &addr_len);

        pthread_t thread;
        pthread_create(&thread, NULL, handle_connection, &client_socket);
    }
}

/**
* Handle connection between client and server
* - send welcome message to newly connected client 
* @param socket is the file descriptor refering the newly connected socket
*/
void* handle_connection(void* socket) 
{
    int client_socket = *(int*) socket;
    last_client_socket = client_socket; // TODO make linked list to handle this shit

    if(client_socket < 0) 
        updateWindow("Server accept failed...");
    else 
    {
        updateWindow("Server accepted a client");
        fflush(stdout);

        char welcome_message[] = "Welcome to server!";
        send(client_socket, welcome_message, sizeof(welcome_message), 0); 
    }

    read_request(client_socket);
}

/** 
* Receive characters from client and write to buffer (ref: main.c)
* @param client_socket is the file descriptor refering the connected socket of the client
*/
void read_request(int client_socket) 
{
    char c;

    for(;;) 
    {
        ssize_t len = read(client_socket, &c, sizeof(c));
        if (len != -1 && len != 0)
            writeToBuffer(c); // main.c
        else 
            break;
    }
}

/**
* Send buffer back to client
* @param buffer that has been modified (send this to client)
* @param len sizeOf(buffer)
*/
void send_buffer(char* buffer, int len)
{
    send(last_client_socket, buffer, sizeof(char)*len, 0);
}

/**
* Print status after attemting to close server socket
*/
void close_server(void)
{
    int status = close(own_socket);
    printf("Attempted closing server with status: %d\n", status);
}
