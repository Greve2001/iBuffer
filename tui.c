#include "common.h"
#include "linklist.h"

WINDOW* statWin;
WINDOW* mainWin;
WINDOW* updateWin;

int strLength;
int xPos;
int xStart = 1;
int yStart = 1;

char* keyword;

void start_tui(char* pass_phrase){
    keyword = pass_phrase;

    int statWidth = 49, statHeight = 4;
    int updateWidth = 49, updateHeight = 4;
    int mainWidth = 100, mainHeight = 50;

    initscr(); cbreak(); noecho(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard
    curs_set(1);

    statWin = newwin(statHeight, statWidth+2, 0, 0);
    updateWin = newwin(updateHeight, updateWidth+2, 0, 51);
    mainWin = newwin(mainHeight, mainWidth+2, 4, 0);

    refresh(); // Important
    print_status();

    xPos = 0;
    strLength = 0;

}

void stop_tui(void){
    werase(statWin);
    werase(updateWin);
    werase(mainWin);
    delwin(statWin);
    delwin(updateWin);
    delwin(mainWin);
    endwin();
}

void buffered_writing(char c){
    curs_set(1);

    // Print
    interpret_char(c);
    char* line1 = get_all_lines()[0];
    strLength = strlen(line1);

    // !!!!!!!!
    wclear(mainWin);
    mvwprintw(mainWin, 1, xStart, "%s", line1); // Print string

    move_cursor(c); // Move cursor

    box(mainWin, 0, 0);
    wrefresh(mainWin);

    //free_list_of_lines(get_all_lines()); // Clean Up
}

void interpret_char(char c){
    // Get better way to handle this.

    // Normal typing
    if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END)){
        write_char(xPos, c);
    }
    else if (c == RETURN){ // Return
        delete_char(xPos);
    }
}

void move_cursor(char c){
    if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END)){ // Normal typing
        if (strLength >= MAX_STRING_LENGTH) return;
        xPos++;
    }
    else if (c == RETURN || c == LEFT_ARROW){ // Return and Arrow Left
        if(xPos > 0) xPos--;
    } 
    else if (c == RIGHT_ARROW){ // Arrow Right
        if (xPos < strLength) xPos++;
    } 

    wmove(mainWin, yStart, xStart+xPos);
}

void print_buffer(char* buffer, int cursorPos){
    curs_set(1);

    wclear(mainWin);
    mvwprintw(mainWin, 1, 1, "%s", buffer);
    box(mainWin, 0, 0);
    wmove(mainWin, 1, cursorPos);
    wrefresh(mainWin);
}

void print_status(void){
    int numUsers = 1;
    curs_set(0);
    wclear(statWin);
    mvwprintw(statWin, 1, xStart, "# Users: \t%d", numUsers);
    mvwprintw(statWin, 2, xStart, "# Keyword:\t%s", keyword);
    box(statWin, 0, 0);
    wrefresh(statWin);
}


int startup_window(void){
    initscr(); cbreak(); noecho(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard
    curs_set(0);

    WINDOW* startupWin = newwin(7, 30, 0, 0);
    refresh();
    
    mvwprintw(startupWin, 1, 1, "Please enter '1', '2' or '3'");
    mvwprintw(startupWin, 3, 1, "1:\tHost Server");
    mvwprintw(startupWin, 4, 1, "2:\tJoin Server");
    mvwprintw(startupWin, 5, 1, "3:\tExit");
    box(startupWin, 0, 0);

    wrefresh(startupWin);

    int returnStatus = -1;
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

    werase(startupWin);
    delwin(startupWin);
    endwin();
    clear();

    return returnStatus;
}

char* input_window(void){
    initscr(); cbreak(); echo(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard
    curs_set(1);

    WINDOW* inputWin = newwin(4, 30, 0, 0);
    refresh();
    
    mvwprintw(inputWin, 1, 1, "Please enter passphrase: ");
    box(inputWin, 0, 0);
    wrefresh(inputWin);

    char* str = malloc(sizeof(char)*100);
    move(2,1); // Move Cursor
    getstr(str);

    werase(inputWin);
    delwin(inputWin);
    endwin();

    return str;
}

void update_window(char* str){
    curs_set(0);

    wclear(updateWin);
    mvwprintw(updateWin, 1, 1, "%s", str);
    box(updateWin, 0, 0);
    wrefresh(updateWin);
}


char* get_buffer(void){
    return get_all_lines()[0]; // Quick fix.
}

int get_cursor_pos(void){
    return xPos;
}
