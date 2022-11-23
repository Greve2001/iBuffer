#include "common.h"

// Global variables, used in different files
int x_cursors[NUMBER_OF_CLI+1];
int y_cursors[NUMBER_OF_CLI+1];

static pthread_mutex_t lock; // Main Lock

int main(void)
{
    srand(time(0)); // Set random seed

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
 * The main function for the server process to run. 
 * - Hosts the buffer for all clients to share
 */
void host(void)
{
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
    for(;;)
    {
        char c = getch();
        if (c == ESCAPE) break; // Escape key

        write_to_buffer(c, NUMBER_OF_CLI);
    }

    // Proper closing of program
    close_program();
}

/**
 * The main function for client processes to run. 
 * - Clients can connect to server processes
 */
void join(void)
{
    char host[NI_MAXHOST] = {0};

    // Ask for passphrase until passphrase is correct or ESCAPE is detected
    while(strnlen(host, NI_MAXHOST) == 0)
    {
        // Input Keyword
        char* key = input_window();
        if (key[0] == ESCAPE) // close program
        {
            free(key);
            stop_tui();
            exit(0);
        }
        // Broadcast
        send_udp_broadcast(host, sizeof(host), key); // for TUI testing
        
        free(key);
    }

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
    stop_tui();
}

/**
 * Takes a character and tries to write it. After writing the new buffer gets broadcasted to all clients
 * @param c The character to write.
 * @param socket_indx The number of the socket calling the method
 */
void write_to_buffer(char c, int socket_indx)
{
    // Lock for writing. Await until unlocked if locked
    pthread_mutex_lock(&lock);

    // Write to buffer, with the requests socket number
    buffered_writing(c, socket_indx); // Calls TUI

    // Fetch the line the socket wrote to, to broadcast
    char* line = get_line(y_cursors[socket_indx]);
    send_buffer(line, strlen(line), socket_indx); // TCP Server

    pthread_mutex_unlock(&lock); // Unlock mutex
}

/**
 * Closes the program properly
 * - this is a responsibility of the server
 */
void close_program(void)
{
    close_server(); 
    pthread_mutex_destroy(&lock);
    stop_tui();
    exit(EXIT_SUCCESS);
}
