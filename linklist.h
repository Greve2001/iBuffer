
//for saving the individual lines
typedef struct Lines{
	struct Lines* next;
	char* paragraph;
} line;


//for saving the active lines
typedef struct Letters{
	struct Letters* next;
	char character;
} letter;


typedef struct Active_Line{
	struct Active_Line* next;	//pointer to next data in the linked list
	line* original_line;		//used to either delete or save it when finished editing
	letter* first_char;			//the first char the paragraph
} active_line;
