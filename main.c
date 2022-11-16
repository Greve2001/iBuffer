#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include "common.h"
#include "linklist.h"

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
    char host[NI_MAXHOST];
    get_local_ip(host);

    printf(" "); // I have no fucking clue why this does fixes a segfault!

    pthread_t thread;
    pthread_create(&thread, NULL, (void*) start_tcp_server, host);

    // Start TUI
    startTUI(pass_phrase);
    updateWindow("");

    // for initial backend setup
    init();

    while (server_running)
    {
        char c = getch();
        if (c == 27) break; // Escape key

        writeToBuffer(c);

    }
    closeProgram();
}

void join(void){
    // Input Keyword
    char* key = malloc(sizeof(char)*100); // Make in tui
    key = inputWindow();

    // Broadcast
    char host[NI_MAXHOST];
    send_udp_broadcast(host, sizeof(host), key); // for TUI testing
    
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
    stopTUI();
}

// Called from Client and Host
// Here we should handle mutex!!!
void writeToBuffer(char c) {
    int status = pthread_mutex_trylock(&lock);
    if (status != 0)
        return;

    bufferedWriting(c); // tui.c
    char* str = getBuffer();
    send_buffer(str, strlen(str)); // tcpserver.c

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
