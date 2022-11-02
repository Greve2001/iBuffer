
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
	struct Active_Line* next;
	letter* first_char;
} active_line;
