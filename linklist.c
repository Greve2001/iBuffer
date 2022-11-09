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
* Methods for changing in between being a linked list and an array
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
* This methods have 2 different uses, these 2 different functionalities are accessible using the bool flag that the method allows as it's second parameter.
*
* Firstly, it allows us to convert any active lines linked list of chars into a string. this is for the front end, so that it can easily call whatever is needed and get an updated string.
*
* Secondly, it is used to also free all the space used for the linked list.
*
* @para active_line, the line wished to getting transformed
* @para free_active_line; if the line needs to stay as a linkedlist or not
*/
void active_line_to_line(Active_Line* active_line, bool free_active_line){
	Line* line = active_line->original_line;
	free(line->paragraph);
	
	int size = active_line->linked_list_size;
	
	if(size == 0){
		;//TODO: nothing on the line
	}
	char* paragraph = malloc((size + 1)* sizeof(char));
	Letter* letter = active_line -> first_char;
	for(int i = 0; i < size; i++){
		paragraph[i] = letter->character;
		if(free_active_line){
			Letter* temp = letter;
			letter = letter -> next;
			free(temp);
		}else
			letter = letter -> next;
	}
	paragraph[size] = '\000';
	line->paragraph = paragraph;
	
	
	if(free_active_line){
		free(line->active_line);
		line->active_line = NULL;
	}
	
}

/******************************************************************
* These methods need public accessability on the local machine
* 
******************************************************************/

/*
* When the front-end register the user clicking on a paragraph, they shall call this.
* This ensures that the char array in converted into a linkedlist and prepared for use.
*
*/
void clicked_on_line(int line_number){
	if (users_active_line){
		active_line_to_line(users_active_line, true);
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
	line_to_active_line(pointer_to_line);
		
}


/*
* Handles the insert of a new character, regardless if it's in the middle or the end of 	the list
*
* param position: the numneric position of the previus element on the line.
* param character: the char that is whished to be written
*/
void write(int position, char character){
	if(!users_active_line)
		return;
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
	if(!users_active_line)
		return;
	if(!users_active_line->first_char){
		//TODO: delete the active line.
	}
	if(position == 0){
		//TODO: merge with previus linkedlist.
	}
	Letter* prev_character = users_active_line->first_char;
	for(int i = 0; i < position-1; i++) prev_character = prev_character->next;
	Letter* after_character = prev_character->next->next;
	free(prev_character->next);
	prev_character->next = after_character;
	users_active_line->linked_list_size--;
}

char* get_line(int line_number){
	Line* line = first_line;
	for(int i = 0; i < line_number; i++) line = line->next;
	if(line->active_line)
		;//TODO: line is current active, so we need to ensure that the string is updated before we try and return it
	return line->paragraph;
}

char** get_all_lines(){
	if(list_of_lines)
		free(list_of_lines);
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
	//this is only for use when the program is closed to free up the memory
}

/***********
* Methods that can be called from the socket module
*
***********/
