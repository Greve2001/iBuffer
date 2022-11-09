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


void host(){
    // Discover
    char* pass_phrase = generate_pass_phrase();
    run_listener();
    // Start TUI
    startTUI(pass_phrase);
    bufferedWriting();
    stopTUI();
}


void join(){
    char ip_addr[NI_MAXHOST];

    // Input Keyword
    char* key = malloc(sizeof(char)*100); // Make in tui
    printf("Insert Key: ");
    scanf("%[^\n]", key);

    // Broadcast
    send_udp_broadcast(ip_addr, NI_MAXHOST-1, key);

    // Start TUI
    
}