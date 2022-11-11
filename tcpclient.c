#include "common.h"

int server_socket;

// TCP connection methods is for now explained in tcpserver.c
void start_tcp_client(char * ip) {

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        updateWindow("Socket creation failed...");
        exit(1);
    }


    // TODO: struct is defined in both client and server, Refactor...
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1504);

    // "Ascii to Network (aton)" and "Network to Ascii (ntoa)" converts IP addresses from a dots-and-number string to a struct in_addr and back
    inet_aton(ip, &server_addr.sin_addr); // Can use INADDR_ANY for automatic filling in the IP

    int status = connect(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(status < 0) {
        updateWindow("Connection with the server failed...");
        exit(1);
    }

    char receive[100];
    read(server_socket, receive, sizeof(receive)); // Reads welcome message
    updateWindow(receive);

    // Listeneer for response from server
    pthread_t thread;
    pthread_create(&thread, NULL, (void*) read_response, NULL);
}

void transfer_msg(char c) {
    send(server_socket, &c, sizeof(c), 0);
}

void read_response(void) {
    for(;;) {
        char argv[100];
        ssize_t len = read(server_socket, argv, sizeof(argv));
        printBuffer(argv);
    }
}

void close_socket(void){
    int status = close(server_socket);
    printf("Tried closing socket with status: %d\n", status);
}
