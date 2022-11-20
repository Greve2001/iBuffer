#include "common.h"
#include "linklist.h"

WINDOW* statWin;
WINDOW* mainWin;
WINDOW* updateWin;

int statWidth = 49, statHeight = 4;
int updateWidth = 49, updateHeight = 4;
int mainWidth = 100, mainHeight = 50;

int strLength;
int xPos, yPos;
int xStart = 1, yStart = 1;

char* keyword;

/**
 * Starts the ncurses Text User Interface.
 * Initializes variables and preset windows.
*/
void start_tui(char* pass_phrase){
    keyword = pass_phrase;

    // Initialization of tui settings and screen
    initscr(); cbreak(); noecho(); 
    keypad(stdscr, TRUE); 
    curs_set(1);

    // Creation of 3 windows
    statWin = newwin(statHeight, statWidth+2, 0, 0);
    updateWin = newwin(updateHeight, updateWidth+2, 0, 51);
    mainWin = newwin(mainHeight, mainWidth+2, 4, 0);

    refresh(); // Refreshes screen

    print_status(); // Display the status window.

    // Set variables
    xPos = 0;
    yPos = 0;
    strLength = 0;
}

/**
 * Stop the TUI by deleting the windows and deallocating them properly
*/
void stop_tui(void){
    werase(statWin);
    werase(updateWin);
    werase(mainWin);
    delwin(statWin);
    delwin(updateWin);
    delwin(mainWin);
    endwin();
}

/**
 * Takes a character and writes it to the buffer.
 * Updates the cursor position and refreshes the screen with new string.
*/
void buffered_writing(char c, int socket_number){
    curs_set(1); // Makes cursor visible

    interpret_char(c, socket_number);
    move_cursor(c, socket_number);

    int* x = &x_cursors[socket_number];
    int* y = &y_cursors[socket_number];

    print_buffer(get_all_lines()[*y], *x, *y);

    // Draw Box outline and refresh
    box(mainWin, 0, 0);
    wrefresh(mainWin);

    //free_list_of_lines(get_all_lines()); // Clean Up
}

/**
 * Interprets the char, if its valid and whether the char is for adding to string,
 * or if its for deleting.
*/
void interpret_char(char c, int socket_number){
    // Get better way to handle this.
    int* x = &x_cursors[socket_number];
    int* y = &y_cursors[socket_number];

    // Normal typing
    if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END))
    {
        write_char(*x, c);
    }
    else if (c == RETURN)
    { // Return
        delete_char(*x);
    }
    else if (c == '\n')
    {
        update_window("New Line!");
        make_new_line(*y);
    }
}

/**
 * Moves the cursor and updates its position variables.
 * (Primarily used internally)
*/
void move_cursor(char c, int socket_number){
    int* x = &x_cursors[socket_number];
    int* y = &y_cursors[socket_number];

    strLength = strlen(get_line(*y));

    if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END))
    { // Normal typing
        if (strLength >= MAX_STRING_LENGTH) return;
        *x++;
    }
    else if (c == RETURN || c == LEFT_ARROW)
    { // Return and Arrow Left
        if(*x > 0) *x--;
    } 
    else if (c == RIGHT_ARROW)
    { // Arrow Right
        if (*x < strLength) *x++;
    }
    else if (c == DOWN_ARROW)
    {
        if (*y > 0)
        {
            *y--; 
            *x = 0;
            clicked_on_line(*y);  
        }
    }
    else if (c == UP_ARROW)
    {
        if (*y < mainHeight-2 && *y < get_amount_of_lines()-1)
        {
            *y++; 
            *x = 0;
            clicked_on_line(*y);  
        }
    }
    else if (c == NEWLINE)
    {
        *y++;
        *x = 0;
        clicked_on_line(*y);  
    }
}

/**
 * Prints the entire buffer on the screen.
 * Takes the cursor position to correctly show where last write or delete was.
*/
void print_buffer(char* buffer, int cursorX, int cursorY){
    curs_set(1); // Cursor visible

    // Clear window and print buffer
    //wclear(mainWin);
    
    /* OLD
    int n_lines = get_amount_of_lines();
    for (size_t i = 0; i < n_lines; i++)
    {
        mvwprintw(mainWin, yStart+i, xStart, "%s", buffer); // Print line
    }
    */

    mvwprintw(mainWin, yStart+cursorY, xStart, "%s", buffer);

    // Draw box and move cursor
    box(mainWin, 0, 0);
    wmove(mainWin, cursorY+yStart, cursorX+xStart);

    wrefresh(mainWin); // Refresh to show updates
}

/**
 * Prints the status window. Displays the amount of users and the keyword for the server
 * This has to be called again for the window to refresh, should the variables have changed.
*/
void print_status(void){
    curs_set(0); // Cursor invisible to not make cursor dart around.

    // Print statuses
    wclear(statWin);
    mvwprintw(statWin, 1, xStart, "# Keyword:\t%s", keyword);

    // Refresh updates
    box(statWin, 0, 0);
    wrefresh(statWin);
}

/**
 * Launches the startup window, that ask the user options for what to do.
 * Returns the integer of the choice selected by user.
 * (start_tui() does not have to be called first)
*/
int startup_window(void){
    // Initial ncurses initialization
    initscr(); cbreak(); noecho();
    keypad(stdscr, TRUE); 
    curs_set(0);

    // Create startup window
    WINDOW* startupWin = newwin(7, 30, 0, 0);
    refresh();
    
    // Print text on window
    mvwprintw(startupWin, 1, 1, "Please enter '1', '2' or '3'");
    mvwprintw(startupWin, 3, 1, "1:\tHost Server");
    mvwprintw(startupWin, 4, 1, "2:\tJoin Server");
    mvwprintw(startupWin, 5, 1, "3:\tExit");
    box(startupWin, 0, 0); // And outline box

    wrefresh(startupWin); // Show updates

    int returnStatus = -1;
    // Get user input, and repeat until accepted
    while(true){
        char c = getch();

        if (c == '1'){
            returnStatus = 0; // Host
            break;
        }
        if (c == '2'){
            returnStatus = 1; // Join
            break;
        }
        if (c == '3'){
            returnStatus = -1; // Exit
            break;
        }
    }

    // Delete window and deallocate
    werase(startupWin);
    delwin(startupWin);
    endwin();
    clear();

    return returnStatus;
}

/**
 * Launches the input window, that asks the user for the keyword.
 * Returns a char* to the inputted string from the user.
 * (start_tui() does not have to be called first)
*/
char* input_window(void){
    // Initial ncurses initialization
    initscr(); cbreak(); echo();
    keypad(stdscr, TRUE); 
    curs_set(1);

    // Create window
    WINDOW* inputWin = newwin(4, 30, 0, 0);
    refresh();
    
    // Print and refresh to display
    mvwprintw(inputWin, 1, 1, "Please enter passphrase: ");
    box(inputWin, 0, 0);
    wrefresh(inputWin);

    // Get string from user
    char* str = malloc(sizeof(char)*100);
    move(2,1); // Move Cursor
    getstr(str);

    // Properly dispose of the window
    werase(inputWin);
    delwin(inputWin);
    endwin();

    return str;
}

/**
 * Refreshes the update window with the inputted string.
 * Used for showing updates to the user.
*/
void update_window(char* str){
    curs_set(0); // Make cursor invisible

    // Print update on the window
    wclear(updateWin);
    mvwprintw(updateWin, 1, 1, "%s", str);
    box(updateWin, 0, 0);

    wrefresh(updateWin); // Show updates
}

/**
 * A middle-man function that returns the buffer, 
 * !which currently is a single line!
*/
char* get_buffer(void){
    return get_all_lines()[yPos]; // Quick fix.
}

/**
 * Provides the cursor's position on screen.
*/
int get_cursor_xPos(void){
    return xPos;
}
int get_cursor_yPos(void){
    return yPos;
}