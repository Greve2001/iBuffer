#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

void send_request(int client_socket) {
    char* line = NULL;
    size_t len = 0;
    ssize_t nread;
    char recv[100];
    for(;;) {
        printf("Ready to recieve command:\n");
        fflush(stdout);
        nread = getline(&line, &len, stdin);

        if(nread == -1) {
            free(line);             // free line even upon failure
            exit(EXIT_FAILURE);
        } 
        else if(nread > 1) {
            send(client_socket, line, sizeof(line), 0);
            read(client_socket, recv, sizeof(recv));
            printf("Contents of host buffer: %s", recv);
        }
    }
    free(line);
    exit(EXIT_SUCCESS);
}


// TCP connection methods is for now explained in tcpserver.c
int main(void) {

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");


    // TODO: struct is defined in both client and server, Refactor...
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1504);

    // "Ascii to Network (aton)" and "Network to Ascii (ntoa)" converts IP addresses from a dots-and-number string to a struct in_addr and back
    inet_aton("127.0.0.1", &server_addr.sin_addr); // Can use INADDR_ANY for automatic filling in the IP
    int status = connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(status != 0) {
        printf("connection with the server failed...%i\n", status);
        exit(0);
    } else {
        printf("connected to the server..\n");
    }
    
    send_request(client_socket);

    close(client_socket);
    
    return 0;
}
