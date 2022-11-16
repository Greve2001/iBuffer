#include "common.h"

int server_socket;

// TCP connection methods is for now explained in tcpserver.c
void start_tcp_client(char * ip) 
{

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) 
    {
        update_window("Socket creation failed...");
        exit(EXIT_FAILURE);
    }


    // TODO: struct is defined in both client and server, Refactor...
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1504);

    // "Ascii to Network (aton)" and "Network to Ascii (ntoa)" converts IP addresses from a dots-and-number string to a struct in_addr and back
    inet_aton(ip, &server_addr.sin_addr);

    if (connect(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) 
    {
        update_window("Connection with the server failed...");
        exit(EXIT_FAILURE);
    }

    char receive[100];
    read(server_socket, receive, sizeof(receive)); // Reads welcome message
    update_window(receive);

    // Listen for response from server
    read_response();
}

void transfer_msg(char c) 
{
    send(server_socket, &c, sizeof(c), 0);
}

// Reads the buffer send by clients
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

void close_socket(void)
{
    int status = close(server_socket);
    printf("Tried closing socket with status: %d\n", status);
}
