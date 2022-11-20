#include "common.h"

int own_socket;
int client_sockets[NUMBER_OF_CLI];

/*
* Start TCP server
* @param ip is the Internet host address
*/
void start_tcp_server(char * ip) 
{
    // endpoint for communication -> file descriptor that refers to the endpoint.
    // * AF_INET is IPv4. SOCK_STREAM provides TCP. Default protocol is 0 when only a single protocol is used.
    own_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (own_socket < 0) 
    {
        update_window("Socket creation failed...");
        exit(1);
    }

    // struct for handling internet addresses defined in <netinit/in.h>. 
    struct sockaddr_in server_addr;

    // set port and address family
    server_addr.sin_family = AF_INET; // TCP
    server_addr.sin_port = htons(1504); // random not reserved TCP socket
    
    // converts from the IPv4 numbers-and-dots notation into binary form
    inet_aton(ip, &server_addr.sin_addr); 
    
    socklen_t addr_len = sizeof(server_addr);  

    // assign address to socket
    if (bind(own_socket, (struct sockaddr*) &server_addr, addr_len) < 0) 
    {
        update_window("Binding to port failed...");
        exit(1);
    }

    // Mark socket as a socket that will listen for connections
    // If the queue is full the client trying to connect will receive ECONNREFUSED    
    if (listen(own_socket, NUMBER_OF_CLI) < 0) 
        update_window("Listening failed...");

    struct sockaddr_in client_addr;
    

    // Extract connection and create a new connected socket and handle this in a new thread
    int clients_connected = 0;
    for(;;)
    {
        // Extract the first connection from the listening socket.
        // Create a new connected socket, and return new file descriptor referring to this socket
        if (clients_connected < NUMBER_OF_CLI)
        {
            client_sockets[clients_connected] = accept(own_socket, (struct sockaddr*) &client_addr, &addr_len);

            pthread_t thread;
            pthread_create(&thread, NULL, (void*) handle_connection, (void*) &clients_connected);

            clients_connected++;
        }
        else
            break;
    }
}

/**
* Handle connection between client and server
* - send welcome message to newly connected client 
* @param socket is the file descriptor refering the newly connected socket
*/
void* handle_connection(void* socket_number) 
{
    int client_socket = client_sockets[*(int*)socket_number];

    if(client_socket < 0) 
        update_window("Server accept failed...");
    else 
    {
        update_window("Server accepted a client");
        fflush(stdout);

        char welcome_message[] = "Welcome to server!";
        send(client_socket, welcome_message, sizeof(welcome_message), 0); 
    }

    read_request(client_socket, *(int*)socket_number);
}

/** 
* Receive characters from client and write to buffer (ref: main.c)
* @param client_socket is the file descriptor refering the connected socket of the client
*/
void read_request(int client_socket, int socket_number) 
{
    Message *pmsg;
    char msg[200] = {0};

    for(;;) 
    {
        ssize_t len = read(client_socket, &msg, sizeof(msg));
        
        if (len != -1 && len != 0)
        {
            pmsg = parser(msg);
            char c = pmsg->message[0];

            if (c == ALT_NEWLINE) 
                c = NEWLINE;

            write_to_buffer(c, socket_number); // main.c
        }
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
    extern int x_cursors[];
    extern int y_cursors[];

    for (size_t i = 0; i < NUMBER_OF_CLI; i++)
    {
        if (&client_sockets[i] != NULL)
        {
            Message msg;
            msg.x = x_cursors[i];
            msg.y = y_cursors[i];
            memset(msg.message, 0, 100);

            if (buffer[0] != '\0')
                strcat(msg.message, buffer);
            else 
                msg.message[0] = ' ';

            char* msg_string = serialize(&msg);
            send(client_sockets[i], msg_string, sizeof(char)*strlen(msg_string), 0);
        }
    }
}

void close_server(void)
{
    int status = close(own_socket);
    printf("Attempted closing server with status: %d\n", status);
}
