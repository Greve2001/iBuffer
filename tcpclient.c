#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

void send_request(int server_socket);

// TCP connection methods is for now explained in tcpserver.c
int main(void) {

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        printf("Socket creation failed...\n");
        exit(1);
    }


    // TODO: struct is defined in both client and server, Refactor...
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1504);

    // "Ascii to Network (aton)" and "Network to Ascii (ntoa)" converts IP addresses from a dots-and-number string to a struct in_addr and back
    inet_aton("127.0.0.1", &server_addr.sin_addr); // Can use INADDR_ANY for automatic filling in the IP

    int status = connect(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(status < 0) {
        printf("Connection with the server failed...%i\n", status);
        exit(1);
    } 

    char receive[100];
    read(server_socket, receive, sizeof(receive)); // Reads welcome message
    printf("Welcome msg: %s", receive);

    send_request(server_socket);

    close(server_socket);

    return 0;
}

void send_request(int server_socket) {
    char* line = NULL;
    size_t len = 0;
    ssize_t nread;
    char recv[100];
    for(;;) {
        nread = getline(&line, &len, stdin);

        if(nread == -1) {
            free(line);             // free line even upon failure
            exit(EXIT_FAILURE);
        } 
        send(server_socket, line, len * sizeof(char), 0);
        read(server_socket, recv, sizeof(recv));
        printf("Contents of host buffer: %s", recv);
        fflush(stdout);
    }
    free(line);
    exit(EXIT_SUCCESS);
}
