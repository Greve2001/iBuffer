#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include "common.h"

int x_cursors[NUMBER_OF_CLI+1];
int y_cursors[NUMBER_OF_CLI+1];

bool server_running = true;

static pthread_mutex_t lock; // Main Lock

int main(void) {
    srand(time(0));

    // Initialize Mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n Mutex init failed\n");
        exit(EXIT_FAILURE);
    }

    // Get user choice
    int returnStatus = startup_window();

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

/**
 * The main function for the server process to run. It hosts the buffer for all clients to share
*/
void host(void){
    // Discover
    char* pass_phrase = generate_pass_phrase();
    run_listener();
    
    // Starting tcp server in a new thread
    char host[NI_MAXHOST];
    get_local_ip(host);
    pthread_t thread;
    pthread_create(&thread, NULL, (void*) start_tcp_server, host);
    
    init(); // Initial Back-end setup

    // Start TUI
    start_tui(pass_phrase);
    update_window("");

    // Listen for escape key to shutdown application
    while (server_running)
    {
        char c = getch();
        if (c == ESCAPE) break; // Escape key

        write_to_buffer(c, NUMBER_OF_CLI);
    }

    // Proper closing of program
    close_program();
}

/**
 * The main function for client processes to run. Can connect to server processes.
*/
void join(void){
    // Input Keyword
    char* key = malloc(sizeof(char)*100); // Make in tui
    key = input_window();

    // Broadcast
    char host[NI_MAXHOST];
    send_udp_broadcast(host, sizeof(host), key); // for TUI testing
    
    pthread_t thread;
    pthread_create(&thread, NULL, (void*) start_tcp_client, host);

    // Start TUI
    start_tui("");
    update_window("");
    for (;;)
    {
        char c = getch(); // Get user keypress

        if (c == ESCAPE) // Escape key
            break;

        // Accept allowed keypresses
        if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END) || c == RETURN || c == LEFT_ARROW || 
            c == RIGHT_ARROW || c == NEWLINE || c == UP_ARROW || c == DOWN_ARROW)
        {
            // Switch out newlines for alternative to bypass tcp ignore
            if (c == NEWLINE) 
                c = ALT_NEWLINE;

            // Setup message with single keypress
            Message msg;
            msg.x = get_cursor_xPos();
            msg.y = get_cursor_yPos(); 
            msg.message[0] = c;
            msg.message[1] = '\0';

            transfer_msg(msg); // Send message
        }
    }

    // Proper closing of loose ends
    close_socket();
    stop_tui();
}

// Called from Client and Host
void write_to_buffer(char c, int socket_number) {
    // Check if mutex is locked. If is, throw away input
    // If not lock mutex
    int status = pthread_mutex_trylock(&lock);
    if (status != 0)
        return;

    // Write to buffer, with the requests socket number
    buffered_writing(c, socket_number); // Calls TUI

    // Fetch the line the socket wrote to, to broadcast
    char* line = get_all_lines()[y_cursors[socket_number]];
    send_buffer(line, strlen(line), socket_number); // tcpserver.c

    pthread_mutex_unlock(&lock); // Unlock mutex
}

/**
 * Closes the program properly
*/
void close_program(void) {
    server_running = false;

    close_socket();
    close_server(); 

    pthread_mutex_destroy(&lock);

    stop_tui();
    exit(EXIT_SUCCESS);
}
