#include "common.h"

int own_socket;
int *client_sockets;
int No_connected_clients = 0;

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
    for(;;)
    {
        // Extract the first connection from the listening socket.
        // Create a new connected socket, and return new file descriptor referring to this socket
        if (No_connected_clients < NUMBER_OF_CLI)
        {
            int client_socket = accept(own_socket, (struct sockaddr*) &client_addr, &addr_len);
            client_sockets = add_to_array(client_sockets, No_connected_clients, client_socket);

            pthread_t thread;
            pthread_create(&thread, NULL, (void*) handle_connection, (void*) &client_socket);

            sleep(1);
            No_connected_clients++;
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
void* handle_connection(void* socket) 
{
    int client_socket = *(int*)socket;

    if(client_socket < 0) 
        update_window("Server accept failed...");
    else 
    {
        update_window("Server accepted a client");
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
    int client_index = get_client_index(client_sockets, No_connected_clients, client_socket);
    printf("%d\n", client_index);

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

            write_to_buffer(c, client_index); // main.c
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

/**
* Add a new element to array
* @param old_array the array to which a new element will be appended
* @param arr_len length of the old array
* @param new_element element to be appended to old_array
* @return a pointer to the new array
*/
int *add_to_array(int *old_array, int arr_len, int new_element) 
{
    int new_array[arr_len + 1];
    for (int i = 0; i < arr_len; i++) 
        new_array[i] = old_array[i];

    new_array[arr_len] = new_element;
    int *new_array_pointer = new_array;
    return new_array_pointer;
}

/**
* Get the index of the client from the client_array that is equal to client_socket
* @param client_array array of clients connected to the server
* @param arr_len number of clients connected to the server
* @param client_socket filedescriptor refering to the socket, which index we're trying to find
* @return index of the client_socket in client_array
*/
int get_client_index(int *client_array, int arr_len, int client_socket) 
{
    for (int i = 0; i < arr_len; i++) 
    {
        //printf("[%d\n]", client_array[i]);
        if (client_array[i] == client_socket)
        {
            return i;
        }
    }
}
