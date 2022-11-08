#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include "common.h"
#define MAX_PASS_LENGTH 1024
#define MAX_PATH_LENGTH 1024
#define RESOURCE_PATH ./resources/
#define RESOURCE_FILES 3

// Networking
#define PORT_NUMBER 2207 
#define BROADCAST_ADDR "0.0.0.0"          // For loopback broadcast
//#define BROADCAST_ADDR "255.255.255.255"    // For network broadcast

static char *server_pass_phrase;
static char *gen_files[] = {"animal_list.txt", "verb_list.txt", "adjective_list.txt"};

void seed_rand(void) 
{
    srand(time(0));
}

void send_udp_broadcast(char ip_server[], int size, char *phrase)
{
    // Create a socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Set the socket options
    int sock_opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &sock_opt, sizeof sock_opt);

    // Settings for sending the broadcast
    struct sockaddr_in broadcast_addr;
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT_NUMBER);
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_ADDR);

    // Send message
    sendto(sock, phrase, strlen(phrase) + 1, 0, (struct sockaddr *) &broadcast_addr, sizeof(broadcast_addr));
    recvfrom(sock, ip_server, size, 0, NULL, 0); 

    close(sock);
}

void listen_udp_broadcast(void) 
{
    // Create listening socket
    int listener = socket(AF_INET, SOCK_DGRAM, 0);

    // Set socket option for broadcast
    int sock_opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_BROADCAST, &sock_opt, sizeof(sock_opt));

    // Set listing struct
    struct sockaddr_in broadcast_addr;
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT_NUMBER);
    broadcast_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the address
    int ret = bind(listener, (struct sockaddr *) &broadcast_addr, sizeof(broadcast_addr));

    // Recieve a message and save who send it
    struct sockaddr_in sender_addr;
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_port = htons(PORT_NUMBER);

    socklen_t sender_size = sizeof(sender_addr);
    char msg[13];
    recvfrom(listener, msg, sizeof(msg) - 1, 0, (struct sockaddr *) &sender_addr, &sender_size); 

    // Check for correct message
    if(strncmp(server_pass_phrase, msg, sizeof(server_pass_phrase) - 2) == 0)
        printf("Validatation approved, from: %s\n", inet_ntoa(sender_addr.sin_addr));

    char local_ip[NI_MAXHOST];
    get_local_ip(local_ip);
    sendto(listener, local_ip, strlen(local_ip) + 1, 0, (struct sockaddr *) &sender_addr, sizeof(sender_addr));
    
    close(listener);
}

// Modified example code from source:
// man page getifaddrs
char get_local_ip(char host[])
{
    struct ifaddrs *ifaddr;
    int family, s;
    host[0] = '\0';

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
    }

    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET && (strstr(ifa->ifa_name, "wl") || strstr(ifa->ifa_name, "eth"))) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
            }

            break;
        }
    }

    freeifaddrs(ifaddr);
    return *host;
}

bool validate_pass_phrase(char *phrase) 
{
    return strncmp(phrase, server_pass_phrase, strnlen(server_pass_phrase, MAX_PASS_LENGTH)) == 0;
}

char *generate_pass_phrase(void) 
{
    free_pass_phrase();

    char *pass_fragment[RESOURCE_FILES];
    size_t size_sum = 0, length = 0;
    for(int i = 0; i < RESOURCE_FILES; i++)
    {
        pass_fragment[i] = get_line_from_file(i, &length);
        size_sum += length;
    }

    if(size_sum > 0)
        server_pass_phrase = (char *) malloc(sizeof(char) * (size_sum + RESOURCE_FILES));
    else
        return "";

    *server_pass_phrase = '\0';
    for(int i = 0; i < RESOURCE_FILES; i++) {
        strcat(server_pass_phrase, pass_fragment[i]);
        free(pass_fragment[i]);
    }

    printf("%s\n", server_pass_phrase);
    return server_pass_phrase;
}

void free_pass_phrase(void)
{
    if (server_pass_phrase) 
    {
        free(server_pass_phrase);
        server_pass_phrase = NULL;
    }
}

char *get_line_from_file(int resource_file_number, size_t *length)
{
    char source[MAX_PATH_LENGTH] = "resources/";
    strncat(source, gen_files[resource_file_number], MAX_PATH_LENGTH - 1);
    FILE *fptr = fopen(source, "r");

    char *line = NULL;
    ssize_t nread = -1;

    // Make sure that the line we try to get is within the length of the file
    int lines = count_lines_in_file(fptr);
    int line_to_get;
    if(lines > 0)
        line_to_get = rand() % lines;
    else
        goto error;

    for(int i = 0; i < line_to_get; i++)
        nread = getline(&line, length, fptr);


    // Check if a reading error has occurred
    if(nread == -1)
        goto error;

    // Remove trailing newline character from the line
    line[strcspn(line, "\n")] = ' ';

error:
    fclose(fptr);
    return line;
}

int count_lines_in_file(FILE *fptr)
{
    if(!fptr)
        return -1;

    char c;
    int lines = 0;
    while((c = fgetc(fptr)) != EOF)
        if(c == '\n')
            lines++;

    // Reset the file pointer af count
    rewind(fptr);
    return lines;
}
