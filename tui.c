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

WINDOW* statWin;
WINDOW* mainWin;

int strLength;
int xPos;
int yStart = 3;

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

    // Refreshing
    refresh();

    xPos = 0;
    strLength = 0;
}

void stopTUI(){
    getch(); // Get char to end! 

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
        printStatus();
        interpretChar(c, str);
        printw("%s", str);
        moveCursor(c);

        //printw("\n%d\n", c); // Print char value

        refresh();
    }
    free(str);
}

void interpretChar(char c, char* str){
    // Get better way to handle this.
    strLength = strlen(str);
    if (strLength >= 99) return;

    // Normal typing
    if ((32 <= c && c <= 126)){
        str[strLength] = c;
        str[strLength+1] = '\0';
    }
    else if (c == 7){ // Return
        str[strLength-1] = '\0';
        str[strLength] = 0;
    }
}

void moveCursor(char c){
    if ((32 <= c && c <= 126)){ // Normal typing
        xPos++;
    }
    else if (c == 7 || c == 4){ // Return and Arrow Left
        if(xPos > 0) xPos--;
    } 
    else if (c == 5){ // Arrow Right
        if (xPos < strLength) xPos++;
    } 

    move(yStart, xPos);
}

void printStatus(){
    int numUsers = 1;
    char* discoveryStr = "Moist-Meat-Sofa";

    printw("# Active Users: \t%d\n", numUsers);
    printw("# Discovery Keyword:\t%s\n", discoveryStr);
    printw("\n");
}



//////// Testing /////////
void testWindow(){
    WINDOW* testWin;

    // Create window
    char* str = "Why you not working!";
    int width = strlen(str)+2;
    int height = 3;

    testWin = newwin(height, width, 0, 0);

    // Append text
    refresh(); // VERY IMPORTANT! Dont know why
    mvwprintw(testWin, 1, 1, "%s", str);
    box(testWin, 0, 0);

    wrefresh(testWin); // Refresh

}
