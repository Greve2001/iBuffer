#include "common.h"
#include <string.h>

/**
 * Convert a string to Message struct. Note have to be correct format for now: {x\n y\n text\n}
 */
Message* parser(char* string) 
{
    Message* message = (Message*) malloc(sizeof(Message));
    char* token;
    char* delim = "\n{}";
    // Get first line of 
    token = strtok(string, delim);
    message->x = atoi(token);
    // Get next line
    token = strtok(NULL, delim);
    message->y = atoi(token);
    token = strtok(NULL, delim);
    strcat(message->message, token); 

    return message;
}

char* serialize(Message* message) 
{
    char *result = (char *) calloc(1, sizeof(*message) + 100);
    result[0] = '{';
    result[1] = '\n';
    result[2] = '\0';
    char conv[5];
    sprintf(conv, "%d\n", message->x);
    strcat(result, conv); 
    sprintf(conv, "%d\n", message->y);
    strcat(result, conv); 
    strcat(result, message->message);
    strcat(result, "\n}");

    return result;
}
