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

#define MAX_STRING_LENGTH   99
#define CHAR_RANGE_START    32
#define CHAR_RANGE_END      126
#define LEFT_ARROW          4
#define RIGHT_ARROW         5
#define RETURN              7

// Structs
struct Active_Line; //Forward decleration

//for saving the individual lines
typedef struct Lines{
	struct Lines* next;
	struct Active_Line* active_line;
	char* paragraph;
} Line;

//for saving the active lines
typedef struct Letters{
	struct Letters* next;
	char character;
} Letter;


typedef struct Active_Line{
	struct Active_Line* next;	//pointer to next data in the linked list
	Line* original_line;		//used to either delete or save it when finished editing
	Letter* first_char;			//the first char the paragraph
	int linked_list_size;
} Active_Line;


// Main
void host(void);
void join(void);
void writeToBuffer(char c);
void closeProgram(void);
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
void testWindow(void);
void bufferedWriting(char);
void printStatus(void);
void interpretChar(char c);
void printBuffer(char*, int);
void moveCursor(char c);
void startTUI(char* pass_phrase);
void stopTUI(void);
int startupWindow(void);
char* inputWindow(void);
void updateWindow(char*);
char* getBuffer(void);
int getCursorPos(void);

// TCP client
void start_tcp_client(char *);
void transfer_msg(char c);
void read_response(void);
void close_socket(void);

// TCP server
void start_tcp_server(char *);
void* handle_connection(void* socket);
void read_request(int client_socket);
void send_buffer(char*, int, int);
void close_server(void);

// Linked List
void make_new_line(int previus_line);
void write_char(int position, char character);
void delete_char(int position);
void line_to_active_line(Line* line);
void active_line_to_line(Active_Line* active_line, bool free_active_line);
void clicked_on_line(int line_number);
char* get_line(int line_number);
char** get_all_lines();
void free_list_of_lines(char** list_to_lines);
void free_all_space(void);
void init(void);
