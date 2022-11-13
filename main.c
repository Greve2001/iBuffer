#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include "common.h"

bool server_running = true;

static pthread_mutex_t lock; // Main Lock

int main(void) {
    srand(time(0));

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Mutex init failed\n");
        exit(EXIT_FAILURE);
    }

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
    pthread_create(&thread, NULL, (void*) start_tcp_server, host);

    // Start TUI
    startTUI(pass_phrase);
    updateWindow("");
    while (server_running)
    {
        char c = getch();
        if (c == 27) break; // Escape key

        writeToBuffer(c);

    }
    closeProgram();
}

void join(void){
    char ip_addr[NI_MAXHOST];

    // Input Keyword
    char* key = malloc(sizeof(char)*100); // Make in tui
    key = inputWindow();

    // Broadcast
    //send_udp_broadcast(ip_addr, NI_MAXHOST-1, key); // for TUI testing

    char *host = "127.0.0.1";
    pthread_t thread;
    pthread_create(&thread, NULL, (void*) start_tcp_client, host);

    // Start TUI
    startTUI("");
    updateWindow("");
    for (;;)
    {
        char c = getch(); // Escape key
        if (c == 27) 
            break;

        if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END) || c == RETURN || c == LEFT_ARROW || c == RIGHT_ARROW)
            transfer_msg(c);
    }

    close_socket();
    if (pthread_join(&thread, NULL) != 0)
        perror("Error joining thread...")
    stopTUI();
}

// Called from Client and Host
// Here we should handle mutex!!!
void writeToBuffer(char c) {
    int status = pthread_mutex_trylock(&lock);
    if (status != 0)
        return;

    bufferedWriting(c);
    char* str = getBuffer();
    send_buffer(str, strlen(str), getCursorPos());

    pthread_mutex_unlock(&lock);
}

void closeProgram(void) {
    server_running = false;

    close_socket();
    close_server();

    pthread_mutex_destroy(&lock);

    stopTUI();
    exit(EXIT_SUCCESS);
}