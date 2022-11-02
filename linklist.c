#include <stdbool.h>
#include <stdlib.h>

#include "linklist.h"

// buffered lines, that are currently not touched
static int size = 0;
static line* first_line;
static line* last_line;


// active lines
static int active_users = 0;
static line* active_first_line;
static line* active_last_line;


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
*
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
