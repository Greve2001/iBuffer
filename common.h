#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

bool validate_pass_phrase(char *);
char *send_udp_broadcast(void);
void listen_udp_broadcat(void);
char *generate_pass_phrase(void);
char *get_line_from_file(int, size_t *);
void free_pass_phrase(void);
int count_lines_in_file(FILE *);
