#include "common.h"

int server_socket;

/** 
* Start TCP client
* @param ip of the server you're trying to connect to 
*/
void start_tcp_client(char * ip) 
{
    // filedescriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) 
    {
        update_window("Socket creation failed...");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    
    // set port and address family  
    server_addr.sin_family = AF_INET; // TCP
    server_addr.sin_port = htons(1504); // random not reserved TCP socket


    // converts from the IPv4 numbers-and-dots notation into binary form
    inet_aton(ip, &server_addr.sin_addr);

    // connect socket to the server address
    if (connect(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) 
    {
        update_window("Connection with the server failed...");
        exit(EXIT_FAILURE);
    }

    // Read and print welcome message to client
    char receive[100];
    read(server_socket, receive, sizeof(receive));
    update_window(receive);

    // Listen for response from server
    read_response();
}

/** 
* Reads the buffer sent from the server
*/
void read_response(void) 
{
    for(;;) 
    {
        char recv[100];
        memset(recv, 0, sizeof(recv));
        ssize_t len1 = read(server_socket, recv, sizeof(recv));

        if (len1 != -1 && len1 != 0)
            print_buffer(recv, 0);
        else
            break;
    }
    pthread_exit(NULL);
}

/**
* Send character to server
* @param c is the character you want to send
*/
void transfer_msg(char c) 
{
    send(server_socket, &c, sizeof(c), 0);
}

/**
* Print status after attemting to close server socket
*/
void close_socket(void)
{
    int status = close(server_socket);
    printf("Tried closing socket with status: %d\n", status);
}
