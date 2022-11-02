#include "common.h"
#include <string.h>
#define MAX_PASS_LENGTH 1024

static char *server_pass_phrase;

bool authenticate(char *phrase) 
{
    return strncmp(phrase, server_pass_phrase, strnlen(server_pass_phrase, MAX_PASS_LENGTH)) == 0;
}

char *generate_pass_phrase(void) 
{
    free_pass_phrase();
    server_pass_phrase = (char *) malloc(sizeof(char));
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
