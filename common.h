#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// Networking headers
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <ncurses.h> // TUI specific

// TCP headers
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// Main
void host(void);
void join(void);

// Discover
bool validate_pass_phrase(char *);
void send_udp_broadcast(char [], int, char *);
pthread_t run_listener(void);
void *listen_udp_broadcast(void);
char get_local_ip(char []);
char *generate_pass_phrase(void);
char *get_line_from_file(int, size_t *);
void free_pass_phrase(void);
int count_lines_in_file(FILE *);

// TUI
void testWindow(void);
void bufferedWriting(char);
void printStatus(void);
void interpretChar(char c, char* str);
void interpretChar2(char c, char* str, int* pos);
void moveCursor(char c);
void startTUI(char* pass_phrase);
void stopTUI(void);
int startupWindow(void);
char* inputWindow(void);

// TCP
void read_request(int client_socket);
void* handle_connection(void* socket);
void transfer_msg(int server_socket);
void start_tcp_server(char *);
void start_tcp_client(char *);
