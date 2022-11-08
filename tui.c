#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void testWindow();
void bufferedWriting();
void printStatus();
void interpretChar(char c, char* str);
void interpretChar2(char c, char* str, int* pos);
void moveCursor(char c);
void startTUI();
void stopTUI();

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


void tuiMain(){
    startTUI();
    bufferedWriting();
    stopTUI();
}

void startTUI(){
    int statWidth = 100, statHeight = 4;
    int mainWidth = 100, mainHeight = 50;

    initscr(); cbreak(); noecho(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard

    statWin = newwin(4, 50, 0, 0);
    mainWin = newwin(50, 102, 4+0, 0);

    // Refreshing
    wrefresh(statWin);
    wrefresh(mainWin);
    refresh();

    xPos = 0;
    strLength = 0;
}

void stopTUI(){
    getch(); // Get char to end! 

    werase(statWin);
    werase(mainWin);
    delwin(statWin);
    delwin(mainWin);
    endwin();
}

void bufferedWriting(){
    char* str = malloc(sizeof(char)*100);

    while (true)
    {
        // Setup counters
        int len = strlen(str);

        // Get input and process it
        char c = getch();

        // Write to window
        clear(); // Clear window

        interpretChar(c, str);

        printStatus();
        mvwprintw(mainWin, 1, xStart, "%s", str); // Print string
        moveCursor(c);

        //printw("\n%d\n", c); // Print char value
        refresh();

        box(statWin, 0, 0);
        box(mainWin, 0, 0);
        wrefresh(statWin);
        wrefresh(mainWin);
    }
    free(str);
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

void printStatus(){
    int numUsers = 1;
    char* discoveryStr = "Moist-Meat-Sofa";

    mvwprintw(statWin, 1, xStart, "# Active Users: \t%d", numUsers);
    mvwprintw(statWin, 2, xStart, "# Discovery Keyword:\t%s", discoveryStr);
}