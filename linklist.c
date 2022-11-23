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
* These methods need public accessability for the logic layer
* 
******************************************************************/

/*
* Creates a new line in the end of the linkedlist.
*
*/
void make_new_line(void)
{
	Line *new = (Line*) malloc(sizeof(Line));	//stack allocate memory for the new line
	new -> next = NULL;							//Ensures the values are 0
	new -> first_letter = NULL;
	new -> list_size = 0;

	//checks if there were any errors with the malloc.
	if(!new)
	{
		;//TODO: error handling for malloc error
	}
	
	lines++;
	
	//checks if this is the first line
	if(!first_line)
	{
		first_line = new;
		last_line = new;
		lines = 1;
		return;
	}
	
	//insert it in the end of the linkedlist
	last_line -> next = new;
	last_line = new;
}

/*
* Handles the insert of a new character, regardless if it's in the middle or the end of 	the list
*
* param letter: the char that is whished to be written
* param letter_position: the numeric position of where the letter should be inserted in the line
* param line_number: on what line the letter should be written
*/
void write_char(char letter, int letter_position, int line_number)
{
	//guard condition: ensures that the loops in the rest doesn't go out of index.
	if(line_number >= lines)
		return;
	
	//iterates through the linkedlist of lines to find the one that should get the new letter.
	Line *working_line = first_line;
	for(int i = 0; i < line_number; i++)
	{
		working_line = working_line -> next;
	}

	//hard locks the line if amount of letters is above 98 (this shouldn't be there in later version)
	//this is due to the current limitation of the TUI.
	if(working_line -> list_size > 98)
		return;
	
	//guard condition: ensures that the loops in the rest doesn't go out of index.
	if(working_line -> list_size < letter_position)
		return;
	
	//If the programs gets here, then it is possible to write the given char.
	//Therefor it now allocates the memory for the letter struct.
	Letter *new_letter = (Letter*) malloc(sizeof(Letter));
	new_letter -> character = letter;
	working_line -> list_size ++;

	//checks if it need to go into the first position, and insert it
	if(letter_position == 0)
	{
		new_letter -> next = working_line -> first_letter;
		working_line -> first_letter = new_letter;
		return;
	}
	
	//iterates over the letters to find the letter prior to where this new one should go
	Letter *before_new_letter = working_line -> first_letter;
	for(int i = 1; i < letter_position; i++)
	{
		before_new_letter = before_new_letter -> next;
	}

	//insert the new letter into the linkedlist
	new_letter -> next = before_new_letter -> next;
	before_new_letter -> next = new_letter;
}


/*
* Deals with the deletion process of a single letter.
*
* @param letter_position: char number in the line.
* @param line_number: What line number that needs to be deleted on.
*/
void delete_char(int letter_position, int line_number){
	
	//guard condition: can't delete the line at the current moment.
	if(letter_position == 0)
		return;

	//iterates to find the correct line struct
	Line *working_line = first_line;
	for(int i = 0; i < line_number; i++)
	{
		working_line = working_line -> next;
	}

	//guard condition: since lines can't be deleted, we can't on an empty linkedlist
	if(working_line -> list_size == 0)
		return;

	//when it's the only char in the linkedlist, the pointers of that linkedlist need to be NULL'ed, else there will be an error when trying to write to it later.
	if(working_line -> list_size == 1)
	{
		free(working_line -> first_letter);
		working_line -> first_letter = NULL;
		working_line -> list_size = 0;
		return;
	}

	working_line -> list_size --;

	//this is used for removing the first char in the list, since this require handling of the Line pointers, too.
	if(letter_position == 1)
	{
		Letter *temp = working_line -> first_letter;
		working_line -> first_letter = temp -> next;
		free(temp);
		return;
	}

	//iterates to find the letter before the one need to delete
	Letter *letter_before_delete = working_line -> first_letter;
	for(int i = 2; i < letter_position; i++)
	{
		letter_before_delete = letter_before_delete -> next;
	}

	//ensures that all pointers are correct setup, and free the now unused memory
	Letter *temp = letter_before_delete -> next;
	letter_before_delete -> next = temp -> next;
	free(temp);

}

/*
* If the users only need to look at one specifik line, this is used for that case.
*
* @param: the line that is wanted to get the string value of.
* @return: a string created by all the chars in the linked list
*/
char* get_line(int line_number){

	//if a previus line have been made, free it first
	if(!one_line)
		free(one_line);
		
	//iterate to find the line that the users wants
	Line *line = first_line;
	for(int i = 0; i < line_number; i++){
		line = line->next;
	}
	
	//allocate the amount of space needed for the string
	//1 extra char is added, this is used later to put a \0 in the end.
	//this allows the caller to operate the return as a string.
	one_line = (char*) malloc((line -> list_size + 1) * sizeof(char));

	//if the line is empty, there will still be returned an empty string.
	if(line -> list_size == 0)
	{
		one_line[0] = '\0';
		return one_line;
	}

	//generates the string, by iterating through all letters, and adding them to the string.
	Letter *temp = line -> first_letter;
	for(int i = 0; i < line -> list_size; i++)
	{
		one_line[i] = temp -> character;
		if(temp -> next)
			temp = temp -> next;
	}
	
	//puts the \0 that allows for the using this as a string.
	one_line[line -> list_size] = '\0';

	return one_line;
}

/*
* Creates an array of strings. This is done by stack allocate a string for each
* linkedlist, and an array for keeping a track of where they are all allocated.
*
* @return a pointer to an array of strings
*/
char **get_all_lines(){
	//If there is already allocated memory for a list_of_lines, it will need to be freed to avoid a memory leak
	if(list_of_lines)
	{
		for(int i = 0; i < list_of_lines_size; i++)
		{
			free(list_of_lines[i]);
		}
		free(list_of_lines);
	}
	
	//Allocated the stack memory for the array of pointers.
	list_of_lines = (char**) malloc(lines * sizeof(char*));
	
	//Since we can't find the size of a dynamic allocated array later, the size is saved.
	//This value is only used for deallocating the memory later.
	list_of_lines_size = lines;

	//Iterates through each line to create the string.
	//The actual string creating is handled by the get_line method, since it already generate the string. But it's required that this method does its own stack allocation, since the allocation done in get_line will be deallocation on next run.
	Line *line = first_line;
	for(int i = 0; i < lines; i++)
	{
		char *temp = (char*) malloc((line -> list_size + 1) * sizeof(char));
		list_of_lines[i] = temp;
		temp = get_line(i);
	}
	return list_of_lines;
}

/*
* This should be called when closing program.
* 
* This will clear all the stack allocated memory and free it to the OS. If this is not
* run at the end of the program, all the malloc allocated memory risk of beign
* kept reserved. In that case, users might have to restart there computer to free
* this memory.
*
* All code here is used elsewhere in the program, and details about how they work can be found in ther respected sections.
*
*/
void free_all_space(void)
{
	//frees all the linkedlists
    for(int i = 0; i< lines; i++)
    {
        Line *temp = first_line;
        Letter *temp2 = temp->first_letter;
        for(int j = 0; j < temp->list_size; j++)
        {
            Letter *delete = temp2;
            temp2 = temp2->next;
            free(delete);
        }
        first_line = temp->next;
        free(temp);
    }
    
    //free the array of strings
    if(list_of_lines)
	{
		for(int i = 0; i < list_of_lines_size; i++)
		{
			free(list_of_lines[i]);
		}
		free(list_of_lines);
	}
	
	//free the string
	if(!one_line)
		free(one_line);
}

/*
* This must always be called first.
*
* Ensures that we get the first linkedlist setup correctly.
*/
void init(void){
	make_new_line();
}

/*
* Can be called if the program that uses this backend needs to know how many lines that exsist.
*
*/
int get_amount_of_lines(void)
{
	return lines;
}
