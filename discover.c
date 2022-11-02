#include "common.h"
#include <stdio.h>
#define MAX_PASS_LENGTH 1024
#define MAX_PATH_LENGTH 1024
#define RESOURCE_PATH ./resources/
#define RESOURCE_FILES 3

static char *server_pass_phrase;
static char *gen_files[] = {"adjective_list.txt", "animal_list.txt", "verb_list.txt"};
int count_lines_in_file(FILE *fptr);
char *get_line_from_file(int resource_file_number, size_t *length);

void seed_rand(void) 
{
    srand(time(0));
}

bool validate_pass_phrase(char *phrase) 
{
    return strncmp(phrase, server_pass_phrase, strnlen(server_pass_phrase, MAX_PASS_LENGTH)) == 0;
}

char *generate_pass_phrase(void) 
{
    free_pass_phrase();

    char *pass_fragment;
    size_t size_sum = 0, length = 0;
    for(int i = 0; i < RESOURCE_FILES; i++)
    {
        pass_fragment = get_line_from_file(i, &length);
        size_sum += length;
    }
    
    if(size_sum > 0)
        server_pass_phrase = (char *) malloc(sizeof(char) * size_sum);
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
    char source[MAX_PATH_LENGTH];
    strncat(source, gen_files[resource_file_number], MAX_PATH_LENGTH);
    FILE *fptr = fopen(source, "r");

    char *line = NULL;
    ssize_t nread;

    // Make sure that the line we try to get is within the length of the file
    int lines = count_lines_in_file(fptr);
    int line_to_get;
    if(lines > 0)
        line_to_get = rand() % lines;
    else
        goto error;

    for(int i = 0; i < line_to_get; i++)
        nread = getline(&line, length, fptr);

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

    return lines;
}
