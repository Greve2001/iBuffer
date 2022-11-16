#include "common.h"

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
		;//TODO: error handling for malloc error
	}
	
	if(!first_line)
		first_line = new;
	else
		last_line->next = new;
	last_line = new;
	size++;
}

/**************************************************************
* Methods for changing in between being a linked list and an array
*
***************************************************************/
void line_to_active_line(Line* line){
	
	//setup new active line in linkedlist
	Active_Line* new_active_line = (Active_Line*) malloc(sizeof(Active_Line));
	users_active_line = new_active_line;
	new_active_line->original_line = line;
	line->active_line = new_active_line;
	
	//checks if there already is an active line or this is the first
	if(active_first_line)
		active_last_line->next = new_active_line;
	else
		active_first_line = new_active_line;
	active_last_line = new_active_line;
	
	if(!line->paragraph){
		new_active_line -> linked_list_size = 0;
		return;
	}
	
	Letter* last_letter = (Letter*) malloc(sizeof(Letter));
	last_letter->character = line->paragraph[0];
	new_active_line->first_char = last_letter;
	new_active_line->linked_list_size = 1;
	for(int i = 1; line->paragraph[i] != '\0'; i++){
		last_letter->next = (Letter*) malloc(sizeof(Letter));
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
	if(line->paragraph)
		free(line->paragraph);
	
	
	int size = active_line->linked_list_size;
	
	if(size == 0){
		line->paragraph = (char*) malloc(sizeof(char));
		line->paragraph[0] = '\0';
		return;
	}
	char* paragraph = (char*) malloc((size + 1)* sizeof(char));
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
void write_char(int position, char character){
	if(!users_active_line)
		return;
	
	//hard locks the line if amount of letters is above 98 (this shouldn't be ther in later version
	if(users_active_line->linked_list_size > 98)
		return;
	
	Letter* new_letter = (Letter*) malloc(sizeof(Letter));
	new_letter -> character = character;
	if(!users_active_line->first_char){
		users_active_line->first_char = new_letter;
		users_active_line->linked_list_size++;
		return;
	}
	Letter* prev_letter = users_active_line->first_char;
	for(int i = 0; i<position-1; i++) prev_letter = prev_letter->next;
	users_active_line->linked_list_size++;
	new_letter->next = prev_letter->next;
	prev_letter->next = new_letter;
	
}


/*
* Deals with the deletion process of a single letter.
*
* Must be parsed the numeric position of the character.
*/
void delete_char(int position){
	if(!users_active_line)
		return;
	if(!users_active_line->first_char){
		return;//TODO: delete the active line.
	}
	if(position > users_active_line->linked_list_size)
		return;
	if(position == 0){
		//TODO: merge with previus linkedlist.
		return;
	}
	if(position == 1){
		Letter* temp = users_active_line->first_char;
		users_active_line->first_char = temp->next;
		free(temp);
		users_active_line->linked_list_size--;
		return;
	}
	Letter* prev_character = users_active_line->first_char;
	for(int i = 0; i < position - 2; i++) prev_character = prev_character->next;
	Letter* after_character = prev_character->next->next;
	free(prev_character->next);
	prev_character->next = after_character;
	users_active_line->linked_list_size--;
}

char* get_line(int line_number){
	Line* line = first_line;
	for(int i = 0; i < line_number; i++) line = line->next;
	
	//checks if the current line is in use, and creates a string of it to print.
	if(line->active_line){
		active_line_to_line(line->active_line,false);
	}
	return line->paragraph;
}

/*
* returns a pointer to an array of all the strings for each line
*
*/
char * * get_all_lines(){
	if(list_of_lines)
		free(list_of_lines);
		
	list_of_lines = (char**) malloc(size * sizeof(char*));
	Line* current_element = first_line;
	for(int i = 0; i < size; i++){
		if(current_element->active_line){
			active_line_to_line(current_element->active_line,false);
		}
		list_of_lines[i] = current_element->paragraph;
		current_element = current_element->next;
	}
	return list_of_lines;
}

/*
* Frees the malloc allocated memory for the pointers to the char* from get_all_lines
*
* This must be called when done using the char** from get_all_lines
*/
void free_list_of_lines(char** list_to_lines){
	if(list_to_lines)
		free(list_to_lines);
}

void free_all_space(void){
	;//TODO: free it all, need to make list of what needs to be free'ed and deal with the seperately
	//this is only for use when the program is closed to free up the memory
}

void init(void){
	make_new_line(0);
    clicked_on_line(0);
}

/***********
* Methods that can be called from the socket module
*
***********/
