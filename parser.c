#include "common.h"
#include <string.h>

Message* parser(char* string) 
{

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
