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


void host(void){
    // Discover
    char* pass_phrase = generate_pass_phrase();
    run_listener();
    
    // Starting tcp server in a new thread
    char host[NI_MAXHOST];
    get_local_ip(host);

    pthread_t thread;
    pthread_create(&thread, NULL, (void*) start_tcp_server, host);
    
    // for initial backend setup
    init();

    // Start TUI
    start_tui(pass_phrase);
    update_window("");


    while (server_running)
    {
        char c = getch();
        if (c == 27) break; // Escape key

        write_to_buffer(c, NUMBER_OF_CLI);

    }
    close_program();
}

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
        char c = getch();
        if (c == 27) // Escape key
            break;

        if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END) || c == RETURN || c == LEFT_ARROW || 
            c == RIGHT_ARROW || c == NEWLINE || c == UP_ARROW || c == DOWN_ARROW)
        {
            if (c == NEWLINE) 
                c = ALT_NEWLINE;

            Message msg;
            msg.x = get_cursor_xPos();
            msg.y = get_cursor_yPos(); 
            msg.message[0] = c;
            msg.message[1] = '\0';
            transfer_msg(msg);
        }
    }

    close_socket();
    stop_tui();
}

// Called from Client and Host
// Here we should handle mutex!!!
void write_to_buffer(char c, int socket_number) {
    int status = pthread_mutex_trylock(&lock);
    if (status != 0)
        return;

    
    char temp[100];
    sprintf(temp, "Sock: %d  xPos: %d", socket_number, x_cursors[socket_number]);
    update_window(temp);
    
    buffered_writing(c, socket_number); // tui.c
    char* str = get_buffer();
    send_buffer(str, strlen(str)); // tcpserver.c

    pthread_mutex_unlock(&lock);
}

void close_program(void) {
    server_running = false;

    close_socket(); // close all sockets
    close_server(); 

    pthread_mutex_destroy(&lock);

    stop_tui();
    exit(EXIT_SUCCESS);
}
