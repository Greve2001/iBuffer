struct Active_Line; //Forward decleration

//for saving the individual lines
typedef struct Lines{
	struct Lines* next;
	struct Active_Line* active_line;
	char* paragraph;
} Line;


//for saving the active lines
typedef struct Letters{
	struct Letters* next;
	char character;
} Letter;


typedef struct Active_Line{
	struct Active_Line* next;	//pointer to next data in the linked list
	Line* original_line;		//used to either delete or save it when finished editing
	Letter* first_char;			//the first char the paragraph
	int linked_list_size;
} Active_Line;
