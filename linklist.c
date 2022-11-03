#include <stdbool.h>
#include <stdlib.h>

#include "linklist.h"

// buffered lines, that are currently not touched
static int size = 0;
static line* first_line;
static line* last_line;


// active lines
static int active_users = 0;
static active_line* active_first_line;
static active_line* active_last_line;

//this users active line
static int users_active_line_length = 0;
static active_line* users_active_line;


/**********************************************************
* These methods is used for handleing the buffered lines only
*
************************************************************/
void make_new_line(void){
	line* new = (line*) malloc(sizeof(line));

	if(!new){
		//TODO: error handling for malloc error
	}
	
	if(!first_line)
		first_line = new;
	else
		last_line->next = new;
	last_line = new;
}

/**************************************************************
* Theses methods are used to manipulate the active lines
*
***************************************************************/
void active_line_size(active_line* my_line){
	letter* current_char = my_line->first_char;
	
}


void make_line_active(int line_number){
	line* line_to_load = first_line;
	for(int i = 0; i < line_number; i++){
		line_to_load = line_to_load->next;
	}
	
	if(!line_to_load){
		//TODO: deal with NULL pointers
	}
	
}


void edit_line(void){
}

void delist_line_for_edit(active_line* line){
	//allocate memory for the new array, and copy over each letter to it
	char* delisted_line = malloc((users_active_line_length + 1) * sizeof(char));
	letter* current_letter = line->first_char;
	for(int i = 0; i < users_active_line_length; i++){
		delisted_line[i] = current_letter->character;
		letter* temp = current_letter;
		current_letter = current_letter->next;
		free(temp);
	}
	delisted_line[users_active_line_length] = '\000';	//allows to operate the char array as a string, this will be used by the front_end to simplify.
	
	//here we need to deallocate the old array before we save the new.
	free(line->original_line->paragraph);				//free the old array space
	line->original_line->paragraph = delisted_line;		//saves the new array to the inactive list
	
	//Then we need to remove the active line from the linked list of active lines
	active_line* current_active_line = active_first_line;
	for(;current_active_line->next != line; current_active_line = current_active_line->next){
		;
	}
	current_active_line->next = line->next;
	
	//finaly we can free up our original line.
	free(line);
	
	
}


/******************************************************************
* These methods need public accessability.
*
******************************************************************/
void enable_line_for_edit(int line_number, bool local_flag){
	if (local_flag){
		if (users_active_line){
			delist_line_for_edit(users_active_line);
			//TODO: make TCP sent a delist to the other clients
		}
		
		
	}
}
