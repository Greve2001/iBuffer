#ifndef _GNU_SOURCE
#define _GNU_SOURCE     	/* To get defns of NI_MAXSERV and NI_MAXHOST */
#endif

#ifndef COMMON_H
#define COMMON_H

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
#include <bits/types/struct_timeval.h>
#include <sys/select.h>
#include <ncurses.h> // TUI specific

#define NUMBER_OF_CLI 		5

#define ESCAPE				27
#define MAX_STRING_LENGTH   99
#define CHAR_RANGE_START    32
#define CHAR_RANGE_END      126
#define NEWLINE				'\n'
#define UP_ARROW			2
#define DOWN_ARROW			3
#define LEFT_ARROW          4
#define RIGHT_ARROW         5
#define RETURN              7
#define ALT_NEWLINE			'`'

// Structs
struct Letters; //Forward decleration

//for saving the individual lines
typedef struct Lines{
	struct Lines *next;
	struct Letters *first_letter;
	int list_size;
} Line;

//for saving the active lines
typedef struct Letters{
	struct Letters* next;
	char character;
} Letter;

typedef struct Message
{
    int x;
    int y;
	bool own;
    char message[100];
} Message;

// Main
void host(void);
void join(void);
void write_to_buffer(char c, int socket_number);
void close_program(void);
void* get_lock(void);

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
void start_tui(char* pass_phrase);
void stop_tui(void);
void interpret_char(char c, int);
void buffered_writing(char, int);
void print_buffer(char* , int, int, bool);
void print_status(void);
int startup_window(void);
char* input_window(void);
void update_window(char*);
void move_cursor(char c, int);
int get_cursor_xPos(void);
int get_cursor_yPos(void);

// TCP client
void start_tcp_client(char *);
void transfer_msg(Message);
void read_response(void);
void close_socket(void);

// TCP server
void start_tcp_server(char *);
void* handle_connection(void* socket_number);
void read_request(int client_socket);
void send_buffer(char*, int, int);
void close_server(void);

// Linked List
void make_new_line();
void write_char(char, int, int);
void delete_char(int, int);
char* get_line(int);
char** get_all_lines();
void free_all_space(void);
void init(void);
int get_amount_of_lines(void);

// Parser/serialize
Message* parser(char* string);
char* serialize(Message* message);
#endif
