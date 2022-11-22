#include "common.h"

// active lines
static int lines = 0;
static Line *first_line;
static Line *last_line;

//for printing lines
static int list_of_lines_size;
static char **list_of_lines;
static char *one_line;


/******************************************************************
* These methods need public accessability on the local machine
* 
******************************************************************/

void make_new_line(void)
{
	Line *new = (Line*) malloc(sizeof(Line));
	new -> next = NULL;
	new -> first_letter = NULL;
	new -> list_size = 0;

	if(!new)
	{
		;//TODO: error handling for malloc error
	}
	lines++;
	
	if(!first_line)
	{
		first_line = new;
		last_line = new;
		lines = 1;
		return;
	}
	
	last_line -> next = new;
	last_line = new;
}

/*
* Handles the insert of a new character, regardless if it's in the middle or the end of 	the list
*
* param position: the numneric position of the previus element on the line.
* param character: the char that is whished to be written
*/
void write_char(char letter, int letter_position, int line_number)
{
	if(line_number >= lines)
		return;
	Line *working_line = first_line;
	for(int i = 0; i < line_number; i++)
	{
		working_line = working_line -> next;
	}

	//hard locks the line if amount of letters is above 98 (this shouldn't be there in later version)
	if(working_line -> list_size > 98)
		return;
	
	Letter *new_letter = (Letter*) malloc(sizeof(Letter));
	new_letter -> character = letter;
	working_line -> list_size ++;
	
	if(letter_position == 0)
	{
		new_letter -> next = working_line -> first_letter;
		working_line -> first_letter = new_letter;
		return;
	}
	
	Letter *before_new_letter = working_line -> first_letter;
	for(int i = 1; i < letter_position; i++)
	{
		before_new_letter = before_new_letter -> next;
	}
	
	new_letter -> next = before_new_letter -> next;
	before_new_letter -> next = new_letter;
}


/*
* Deals with the deletion process of a single letter.
*
* Must be parsed the numeric position of the character.
*/
void delete_char(int letter_position, int line_number){
	if(letter_position == 0)
		return;
	
	Line *working_line = first_line;
	for(int i = 0; i < line_number; i++)
	{
		working_line = working_line -> next;
	}
	
	if(working_line -> list_size == 0)
		return;
		
	if(working_line -> list_size == 1)
	{
		free(working_line -> first_letter);
		working_line -> first_letter = NULL;
		working_line -> list_size = 0;
		return;
	}
	
	working_line -> list_size --;
		
	if(letter_position == 1)
	{
		Letter *temp = working_line -> first_letter;
		working_line -> first_letter = temp -> next;
		free(temp);
		return;
	}
	
	Letter *letter_before_delete = working_line -> first_letter;
	for(int i = 2; i < letter_position; i++)
	{
		letter_before_delete = letter_before_delete -> next;
	}
	
	Letter *temp = letter_before_delete -> next;
	letter_before_delete -> next = temp -> next;
	free(temp);
		
}

char* get_line(int line_number){
	if(!one_line)
		free(one_line);
	Line *line = first_line;
	for(int i = 0; i < line_number; i++){
		line = line->next;
	}
	
	one_line = (char*) malloc((line -> list_size + 1) * sizeof(char));
	
	if(line -> list_size == 0)
	{
		one_line[0] = '\0';
		return one_line;
	}
	
	Letter *temp = line -> first_letter;
	for(int i = 0; i < line -> list_size; i++)
	{
		one_line[i] = temp -> character;
		if(temp -> next)
			temp = temp -> next;
	}
	one_line[line -> list_size] = '\0';
	
	return one_line;
}

/*
* returns a pointer to an array of all the strings for each line
*
*/
char **get_all_lines(){
	if(list_of_lines)
	{
		for(int i = 0; i < list_of_lines_size; i++)
		{
			free(list_of_lines[i]);
		}
		free(list_of_lines);
	}
	
	list_of_lines = (char**) malloc(lines * sizeof(char*));
	list_of_lines_size = lines;
	
	Line *line = first_line;
	for(int i = 0; i < lines; i++)
	{
		char *temp = (char*) malloc((line -> list_size + 1) * sizeof(char));
		list_of_lines[i] = temp;
		temp = get_line(i);
	}
	return list_of_lines;
}

void free_all_space(void)
{
	;
}

void init(void){
	make_new_line();
}

int get_amount_of_lines(void)
{
	return lines;
}

/***********
* Methods that can be called from the socket module
*
***********/
