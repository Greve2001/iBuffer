#include "common.h"

#define MAX_STRING_LENGTH   99
#define CHAR_RANGE_START    32
#define CHAR_RANGE_END      126
#define LEFT_ARROW          4
#define RIGHT_ARROW         5
#define RETURN              7

WINDOW* statWin;
WINDOW* mainWin;

int strLength;
int xPos;
int xStart = 1;
int yStart = 1;

char* keyword;
char* str;

void startTUI(char* pass_phrase){
    keyword = pass_phrase;
    str = malloc(sizeof(char)*100);

    int statWidth = 100, statHeight = 4;
    int mainWidth = 100, mainHeight = 50;

    initscr(); cbreak(); noecho(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard
    curs_set(1);

    statWin = newwin(statHeight, statWidth+2, 0, 0);
    mainWin = newwin(mainHeight, mainWidth+2, 4+0, 0);

    // Refreshing
    refresh();
    wrefresh(statWin);
    wrefresh(mainWin);

    xPos = 0;
    strLength = 0;
}

void stopTUI(void){
    werase(statWin);
    werase(mainWin);
    delwin(statWin);
    delwin(mainWin);
    endwin();

    free(str);
}

void bufferedWriting(char c){
    // Setup counters
    int len = strlen(str);

    clear(); // Clear window

    printStatus(); // Print status

    // Print
    interpretChar(c, str);
    mvwprintw(mainWin, 1, xStart, "%s", str); // Print string

    moveCursor(c); // Move cursor

    refresh();

    box(statWin, 0, 0);
    box(mainWin, 0, 0);
    wrefresh(statWin);
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

void printStatus(void){
    int numUsers = 1;

    mvwprintw(statWin, 1, xStart, "# Active Users: \t%d", numUsers);
    mvwprintw(statWin, 2, xStart, "# Discovery Keyword:\t%s", keyword);
}


int startupWindow(void){
    initscr(); cbreak(); noecho(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard
    curs_set(0);

    WINDOW* startupWin = newwin(7, 30, 0, 0);
    box(startupWin, 0, 0);
    refresh();
    
    mvwprintw(startupWin, 1, 1, "Please enter '1', '2' or '3'");
    mvwprintw(startupWin, 3, 1, "1:\tHost Server");
    mvwprintw(startupWin, 4, 1, "2:\tJoin Server");
    mvwprintw(startupWin, 5, 1, "3:\tExit");
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

    return returnStatus;
}

char* inputWindow(void){
    initscr(); cbreak(); noecho(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard
    curs_set(1);

    WINDOW* inputWin = newwin(4, 30, 0, 0);
    box(inputWin, 0, 0);
    refresh();
    
    mvwprintw(inputWin, 1, 1, "Please enter passphrase: ");

    wrefresh(inputWin);

    char* str = malloc(sizeof(char)*100);
    getstr(str);

    werase(inputWin);
    delwin(inputWin);
    endwin();

    return str;
}
