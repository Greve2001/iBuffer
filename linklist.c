#include <stdbool.h>
#include <stdlib.h>

#include "linklist.h"

// buffered lines
static int size = 0;
static Line* first_line;
static Line* last_line;


// active lines
static int active_users = 0;
static Active_Line* active_first_line;
static Active_Line* active_last_line;

//this users active line
static Active_Line* users_active_line;

//for printing lines
static char** list_of_lines;


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
void line_to_active_line(Line* line){
	Active_Line* new_active_line = malloc(sizeof(Active_Line));
	new_active_line->original_line = line;
	active_last_line->next = new_active_line;
	active_last_line = new_active_line;
	line->active_line = new_active_line;
	
	if(!line->paragraph){
		//TODO: does not contain any letters, must be handled differently
		return;
	}
	
	Letter* last_letter = malloc(sizeof(Letter));
	last_letter->character = line->paragraph[0];
	new_active_line->first_char = last_letter;
	new_active_line->linked_list_size = 1;
	for(int i = 1; line->paragraph[i] != '\000'; i++){
		last_letter->next = malloc(sizeof(Letter));
		last_letter = last_letter->next;
		last_letter->character = line->paragraph[i];
		new_active_line->linked_list_size++;
	}

}
/*
//TODO: delete
void make_line_active(int line_number){
	Line* line_to_load = first_line;
	for(int i = 0; i < line_number; i++){
		line_to_load = line_to_load->next;
	}
	
	if(!line_to_load){
		//TODO: deal with NULL pointers
	}
	
}

//TODO: delete
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

//TODO: delete
void delist_line_for_edit(Active_Line* line){
	//allocate memory for the new array, and copy over each letter to it
	char* delisted_line = malloc((line->linked_list_size + 1) * sizeof(char));
	Letter* current_letter = line->first_char;
	for(int i = 0; i < line->linked_list_size; i++){
		delisted_line[i] = current_letter->character;
		Letter* temp = current_letter;
		current_letter = current_letter->next;
		free(temp);
	}
	delisted_line[line->linked_list_size] = '\000';	//allows to operate the char array as a string, this will be used by the front_end to simplify.
	
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
*/

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


/*
* Handles the insert of a new character, regardless if it's in the middle or the end of 	the list
*
* param position: the numneric position of the previus element on the line.
* param character: the char that is whished to be written
*/
void write(int position, char character){
	if(character = '\n'){
		//TODO: create new line method
		return;
	}
	Letter* new_letter = malloc(sizeof(Letter));
	new_letter -> character = character;
	Letter* prev_letter = users_active_line->first_char;
	for(int i = 0; i<position; i++) prev_letter = prev_letter->next;
	users_active_line->linked_list_size++;
	new_letter->next = prev_letter->next;
	prev_letter->next = new_letter;
}


/*
* Deals with the deletion process of a single letter.
*
* Must be parsed the numeric position of the character.
*/
void delete(int position){
	Letter* prev_character = users_active_line->first_char;
	for(int i = 0; i < position-1; i++) prev_character = prev_character->next;
	Letter* after_character = prev_character->next->next;
	free(prev_character->next);
	prev_character->next = after_character;
}

char* get_line(int line_number){
	Line* line = first_line;
	for(int i = 0; i < line_number; i++) line = line->next;
	if(line->active_line)
		;//TODO: line is current active, so we need to ensure that the string is updated before we try and return it
	return line->paragraph;
}

char** get_all_lines(){
	list_of_lines = malloc(size * sizeof(char*));
	Line* current_element = first_line;
	for(int i = 0; i < size; i++){
		if(current_element->active_line){
			//TODO: is active, so we need to update the string before returning it
		}
		list_of_lines[i] = current_element->paragraph;
		current_element = current_element->next;
	}
}

void free_all_space(){
	;//TODO: free it all, need to make list of what needs to be free'ed and deal with the seperately
}

/***********
* Methods that can be called from the socket module
*
***********/
