#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include "common.h"

int main(void) {
    srand(time(0));

    int returnStatus = startupWindow();
    switch (returnStatus)
    {
    case 0: // Host
        host();
        break;
    case 1: // Join
        join();
        break;

    default: // Exit
        exit(EXIT_SUCCESS);
        break;
    }

    return EXIT_SUCCESS;
}


void host(void){
    // Discover
    char* pass_phrase = generate_pass_phrase();
    run_listener();
    
    // Starting tcp server in a new thread
    char *host = "127.0.0.1";
    pthread_t thread;
    pthread_create(&thread, NULL, start_tcp_server(host), NULL);

    // Start TUI
    startTUI(pass_phrase);
    while (true)
    {
        char c = getch();
        if (c == 27) break; // Escape key
        updateWindow("Test");
        bufferedWriting(c);
    }
    stopTUI();
}


void join(void){
    char ip_addr[NI_MAXHOST];

    // Input Keyword
    char* key = malloc(sizeof(char)*100); // Make in tui
    key = inputWindow();

    // Broadcast
    //send_udp_broadcast(ip_addr, NI_MAXHOST-1, key); // for TUI testing


    char *host = "127.0.0.1";
    //start_tcp_client(host); // for TUI testing

    // Start TUI
    
}
