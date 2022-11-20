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

char *clients[NUMBER_OF_CLI]; // Added this to make it run

pthread_mutex_t clients_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clients_cond = PTHREAD_COND_INITIALIZER;

/**
 * Sends a broadcast via udp and then waits to an IP address as a response. If no response is
 * given within one second the function closes the listening socket. The returned ip address
 * is the counterpart udp server.
 * @param ip_server char array to hold the server address
 * @param size size of the given array
 * @param phrase pointer to the pass phrase which should be used for server validation
 */
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

    fd_set set;
    struct timeval timeout = {1, 0};

    FD_ZERO(&set);
    FD_SET(sock, &set);

    // Send message
    sendto(sock, phrase, strlen(phrase) + 1, 0, (struct sockaddr *) &broadcast_addr, sizeof(broadcast_addr));
    // Use select to time out socket
    // source https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html
    if(select(FD_SETSIZE, &set, NULL, NULL, &timeout))
        recvfrom(sock, ip_server, size, 0, NULL, 0); 

    close(sock);
}

/**
 * Spawn a new thread to run the udp listener on.
 * @return the thread id of the newly spawned thread
 */
pthread_t run_listener(void)
{
    pthread_t ptid;
    pthread_create(&ptid, NULL, (void *) &listen_udp_broadcast, NULL);

    return ptid;
}

/**
 * Listen for broadcast messages and answer back with the IP of the host to clients
 * only if the have the correct pass phrase.
 * @return NULL
 */
void *listen_udp_broadcast(void) 
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
    char msg[MAX_PASS_LENGTH];
    int c = 0;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(listener, &set);

    for(;;)
    {
        if(select(FD_SETSIZE, &set, NULL, NULL, NULL))
            recvfrom(listener, msg, sizeof(msg), 0, (struct sockaddr *) &sender_addr, &sender_size); 

        // Check for correct message
        if(strncmp(server_pass_phrase, msg, strlen(server_pass_phrase) - 1) == 0)
        {
            //printf("Validatation approved, from: %s\n", inet_ntoa(sender_addr.sin_addr));
            char *tmp = (char *) malloc(sizeof(char) * strlen(inet_ntoa(sender_addr.sin_addr)));
            tmp = inet_ntoa(sender_addr.sin_addr);
            
            // Temporary limitation of number of clients
            if(c >= NUMBER_OF_CLI)
                break;

            pthread_mutex_lock(&clients_lock);
            clients[c++] = tmp; 
            pthread_mutex_unlock(&clients_lock);
            pthread_cond_signal(&clients_cond);

            char local_ip[NI_MAXHOST];
            get_local_ip(local_ip);
            sendto(listener, local_ip, strlen(local_ip) + 1, 0, (struct sockaddr *) &sender_addr, sizeof(sender_addr));
        }
    }

    close(listener);
    return NULL;
}

/**
 * Gets the ip address of the host and is based on the man page getifaddrs example code.
 * @param host char array to hold the host address
 * @return a pointer to the host array
 */
char get_local_ip(char host[])
{
    struct ifaddrs *ifaddr;
    int family, s;
    host[0] = '\0';

    if (getifaddrs(&ifaddr) == -1) 
    {
        perror("getifaddrs");
    }

    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET && (strstr(ifa->ifa_name, "wl") || strstr(ifa->ifa_name, "eth"))) 
        {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

            if (s != 0) 
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
            }

            break;
        }
    }

    freeifaddrs(ifaddr);
    return *host;
}

/**
 * Check if the passed phrase is the same as the currently generated one.
 * @param phrase to validate
 * @return true if they are equal and false if not
 */
bool validate_pass_phrase(char *phrase) 
{
    return strncmp(phrase, server_pass_phrase, strnlen(server_pass_phrase, MAX_PASS_LENGTH)) == 0;
}

/**
 * Generates a new pass phrase based on three txt files and if one is already generated it frees the old
 * one from memory before generating new one
 * @return pointer to the pass phrase on the server
 */
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
    for(int i = 0; i < RESOURCE_FILES; i++) 
    {
        strcat(server_pass_phrase, pass_fragment[i]);
        free(pass_fragment[i]);
    }

    //printf("%s\n", server_pass_phrase);
    return server_pass_phrase;
}

/**
 * Free the currently allocated pass phrase
 */
void free_pass_phrase(void)
{
    if (server_pass_phrase) 
    {
        free(server_pass_phrase);
        server_pass_phrase = NULL;
    }
}

/**]
 * Get a sub part of the pass phrase from one of the resource files defined in this module.
 * @param resource_file_number which resource to pick from the array
 * @param length the length of the found line is written when found
 * @return pointer to the line
 */
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
    
    nread = getline(&line, length, fptr);
    for(int i = 0; i < line_to_get - 1; i++)
        nread = getline(&line, length, fptr);


    // Check if a reading error has occurred
    if(nread == -1)
        goto error;
    
    int first = 0;
    // Remove trailing newline character from the line
    for(int i = 0; i < nread; i++)
    {
        char c = line[i];
        if(c == '\n' || c == '\r' || c == ' ')
        {
            if(first == 0)
                first = i;
            line[i] = '\0';
        }
    }

    line[first] = ' ';

error:
    fclose(fptr);
    return line;
}

/**
 * Count the number of lines in a file
 * @param fptr an open file pointer
 * @return the number of lines as a integer
 */
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
