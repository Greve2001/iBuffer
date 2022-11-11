#include "common.h"

int server_socket;

// TCP connection methods is for now explained in tcpserver.c
void start_tcp_client(char * ip) {

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        //printf("Socket creation failed...\n");
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
        //printf("Connection with the server failed...%i\n", status);
        updateWindow("Connection with the server failed...");
        exit(1);
    } 

    char receive[100];
    read(server_socket, receive, sizeof(receive)); // Reads welcome message
    //printf("Welcome msg: %s", receive);
    updateWindow(receive);

    transfer_msg(server_socket);
}

void transfer_msg(int server_socket) {
    char line[10] = "exit\n"; // hardcoded
    char recv[100];

    memset(recv, 0, sizeof(recv));
    send(server_socket, line, sizeof(line), 0);
    read(server_socket, recv, sizeof(recv));
    //printf("Contents of host buffer: %s", recv);
}

void close_socket(void){
    int status = close(server_socket);
    printf("Tried closing socket with status: %d\n", status);
}
