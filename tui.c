#include "common.h"

WINDOW* statWin;
WINDOW* mainWin;
WINDOW* updateWin;

int strLength;
int xPos;
int xStart = 1;
int yStart = 1;

char* keyword;
char* str;

void startTUI(char* pass_phrase){
    keyword = pass_phrase;
    str = malloc(sizeof(char)*100);

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
    printStatus();

    xPos = 0;
    strLength = 0;

}

void stopTUI(void){
    werase(statWin);
    werase(updateWin);
    werase(mainWin);
    delwin(statWin);
    delwin(updateWin);
    delwin(mainWin);
    endwin();

    free(str);
}

void bufferedWriting(char c){
    // Setup counters
    int len = strlen(str);
    curs_set(1);

    // Print
    interpretChar(c, str);
    mvwprintw(mainWin, 1, xStart, "%s", str); // Print string

    moveCursor(c); // Move cursor

    box(mainWin, 0, 0);
    wrefresh(mainWin);
}

void interpretChar(char c, char* str){
    // Get better way to handle this.
    strLength = strlen(str);

    // Normal typing
    if ((CHAR_RANGE_START <= c && c <= CHAR_RANGE_END)){
        if (strLength >= MAX_STRING_LENGTH) return;
        str[xPos] = c;
        str[xPos+1] = '\0';
    }
    else if (c == RETURN){ // Return
        str[xPos+1-1] = '\0';
        str[xPos+1] = 0;
    }
}

void moveCursor(char c){
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

void printBuffer(char* buffer, int cursorPos){
    curs_set(1);

    wclear(mainWin);
    mvwprintw(mainWin, 1, 1, "%s", buffer);
    box(mainWin, 0, 0);
    wmove(mainWin, 1, cursorPos);
    wrefresh(mainWin);
}

void printStatus(void){
    int numUsers = 1;
    curs_set(0);
    mvwprintw(statWin, 1, xStart, "# Users: \t%d", numUsers);
    mvwprintw(statWin, 2, xStart, "# Keyword:\t%s", keyword);
    box(statWin, 0, 0);
    wrefresh(statWin);
}


int startupWindow(void){
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

char* inputWindow(void){
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

void updateWindow(char* str){
    curs_set(0);

    wclear(updateWin);
    mvwprintw(updateWin, 1, 1, "%s", str);
    box(updateWin, 0, 0);
    wrefresh(updateWin);
}


char* getBuffer(void){
    return str;
}

int getCursorPos(void){
    return xPos;
}