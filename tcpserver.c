#include "common.h"

int own_socket;
int last_client_socket; // Fix

bool running;

void start_tcp_server(char * ip) {

    /**
     * socket(int domain, int type, int protocol) creates an endpoint for communication and returns a file descriptor that refers to that endpoint.
     * int domain - the protocol family which will be used for communication defined in <sys/socket.h>. AF_INET is IPv4.
     * int type - specifies the communication semantics. SOCK_STREAM provides sequenced, reliable, two-way, connection-based byte streams (TCP).
     * int protocol - specifies a particular protocol to be used with the socket. Default is 0 when only a single protocol is used. Can be used to define a socket with several different protocols.
     */
    own_socket = socket(AF_INET, SOCK_STREAM, 0); // filedescriptor
    running = true;

    if (own_socket < 0) {
        updateWindow("Socket creation failed...");
        exit(1);
    }

    /* struct for handling internet addresses defined in <netinit/in.h>. 
     * struct sockaddr_in {
     *      short            sin_family;   // e.g. AF_INET
     *      unsigned short   sin_port;     // e.g. htons(3490). Has something to do with network byte ordering, where we use little endian (most significant byte in first addresses).
     *      struct in_addr   sin_addr;     // see struct in_addr, below
     *      char             sin_zero[8];  // zero this if you want to
     *  };
     *  
     *  struct in_addr {
     *       unsigned long s_addr;  // load with inet_aton()
     *  };
     */
    struct sockaddr_in server_addr;

    // Set port and IP
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1504);

    // "Ascii to Network (aton)" and "Network to Ascii (ntoa)" converts IP addresses from a dots-and-number string to a struct in_addr and back
    inet_aton(ip, &server_addr.sin_addr); // Can use INADDR_ANY for automatic filling in the IP

    /**
     * int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen) assigns the address specified by addr to the socket referred to by the file descriptor sockfd.
     * int sockfd - the file descriptor returned from socket()
     * struct addr - the adress handled by the struct sockaddr_in
     * socklen_t addrlen - the length of the struct
     */
    if (bind(own_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        updateWindow("Binding to port failed...");
        exit(1);
    }
    /**
     * listen(int sockfd, int backlog) mark the socket as passive to accept incoming requests.
     * int sockfd - the file descriptor return from socket()
     * int backlog - the length of the queue of incoming requests. If connection is full the client recieve ECONNREFUSED. 
     */
    // Listen for clients
    if (listen(own_socket, 5) < 0) {
        updateWindow("Listening failed...");
    }

    socklen_t addr_len = sizeof(server_addr);    

    struct sockaddr_in client_addr;

    while(running) {
        /**
         * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) - extracts the first connection request on the queue of the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.
         * int sockfd - the file descriptor returned from socket(), and has bind() and listen().
         * struct addr - the adress handled by the struct sockaddr_in
         * socklen_t addrlen - must initialize it to contain the size (in bytes) of the structure pointed to by addr; on return it will contain the actual size of the peer address.
         */
        int client_socket = accept(own_socket, (struct sockaddr*) &client_addr, &addr_len);

        pthread_t thread;
        pthread_create(&thread, NULL, handle_connection, &client_socket);

    }
}

void* handle_connection(void* socket) {
    int client_socket = *(int*) socket;
    last_client_socket = client_socket; // TODO make linked list to handle this shit

    if(client_socket < 0) {
        updateWindow("Server accept failed");
    } else {
        updateWindow("Server accepted a client");
        fflush(stdout);
        char welcome_message[] = "Welcome to server. Ready to recieve a command.";
        send(client_socket, welcome_message, sizeof(welcome_message), 0); 
    }

    read_request(client_socket);
}

// Receives chars from client
void read_request(int client_socket) {
    char c;
    for(;;) {
        ssize_t len1 = read(client_socket, &c, sizeof(c));
        if (len1 != -1 && len1 != 0)
            writeToBuffer(c);
        else
            break;
    }
    closeProgram();
}

void send_buffer(char* buffer, int cursor_x){
    send(last_client_socket, buffer, sizeof(buffer), 0);
    //send(last_client_socket, &cursor_x, sizeof(cursor_x), 0); // Does not work
}

void close_server(void){
    int status = close(own_socket);
    running = false;
    printf("Attempted closing server with status: %d\n", status);
}