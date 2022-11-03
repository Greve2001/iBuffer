#include <stdbool.h>
#include <stdlib.h>

#include "linklist.h"

// buffered lines, that are currently not touched
static int size = 0;
static Line* first_line;
static Line* last_line;


// active lines
static int active_users = 0;
static Active_Line* active_first_line;
static Active_Line* active_last_line;

//this users active line
static int users_active_line_length = 0;
static Active_Line* users_active_line;


/**********************************************************
* These methods is used for handleing the buffered lines only
*
************************************************************/
void make_new_line(int previus_line){
	Line* new = (Line*) malloc(sizeof(Line));

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
void active_line_size(Active_Line* my_line){
	Letter* current_char = my_line->first_char;
	
}


void make_line_active(int line_number){
	Line* line_to_load = first_line;
	for(int i = 0; i < line_number; i++){
		line_to_load = line_to_load->next;
	}
	
	if(!line_to_load){
		//TODO: deal with NULL pointers
	}
	
}


void list_line_for_edit(Line* li){
	Active_Line* new_line= malloc(sizeof(Active_Line));
	li->active_line = new_line;
	new_line->original_line = li;
	
	Letter* last_inserted_letter;
	for(int i = 0; li->paragraph[i] != '\000'; i++){
		Letter* l = malloc(sizeof(Letter));
		l->character = li->paragraph[i];
		if(last_inserted_letter)
			last_inserted_letter->next = l;
		else
			new_line->first_char = l;
		last_inserted_letter = l;
		
	}
	
}

void delist_line_for_edit(Active_Line* line){
	//allocate memory for the new array, and copy over each letter to it
	char* delisted_line = malloc((users_active_line_length + 1) * sizeof(char));
	Letter* current_letter = line->first_char;
	for(int i = 0; i < users_active_line_length; i++){
		delisted_line[i] = current_letter->character;
		Letter* temp = current_letter;
		current_letter = current_letter->next;
		free(temp);
	}
	delisted_line[users_active_line_length] = '\000';	//allows to operate the char array as a string, this will be used by the front_end to simplify.
	
	//here we need to deallocate the old array before we save the new.
	free(line->original_line->paragraph);				//free the old array space
	line->original_line->paragraph = delisted_line;		//saves the new array to the inactive list
	
	//Then we need to remove the active line from the linked list of active lines
	Active_Line* current_active_line = active_first_line;
	for(;current_active_line->next != line; current_active_line = current_active_line->next){
		;
	}
	current_active_line->next = line->next;
	line->original_line->active_line = NULL;
	
	//finaly we can free up our original line.
	free(line);
}


/******************************************************************
* These methods need public accessability on the local machine
* 
******************************************************************/
void clicked_on_line(int line_number){
	if (users_active_line){
		delist_line_for_edit(users_active_line);
		//TODO: make TCP sent a delist to the other clients
	}
	if(!line_number){ //if NULL is passed as parameter
		users_active_line = NULL;
		return;
	}
	Line* pointer_to_line = first_line;
	for(int i = 0; i < line_number; i++){
		pointer_to_line = pointer_to_line->next;
	}
	list_line_for_edit(pointer_to_line);
		
}

void write(int position, char character){
	if(character = '\n'){
		//TODO: create new line method
		return;
	}
	Letter* new_letter = malloc(sizeof(Letter));
	new_letter -> character = character;
	Letter* prev_letter = users_active_line->first_char;
	for(int i = 0; i<position; i++) prev_letter = prev_letter->next;
	new_letter->next = prev_letter->next;
	prev_letter->next = new_letter;
}

void delete(int position){
	Letter* prev_character = users_active_line->first_char;
	for(int i = 0; i < position-1; i++) prev_character = prev_character->next;
	Letter* after_character = prev_character->next->next;
	free(prev_character->next);
	prev_character->next = after_character;
}

char* get_line(int line_number){
	
}

/***********
* Methods that can be called from the socket
*
***********/
