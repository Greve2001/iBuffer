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
void startTUI();
void stopTUI();

WINDOW* statWin;
WINDOW* mainWin;

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
}

void stopTUI(){
    getch(); // Get char to end! 

    endwin();
}

void bufferedWriting(){
    char* str = malloc(sizeof(char)*100);
    //int pos = 0;

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

        refresh();
    }
    free(str);
}

void interpretChar(char c, char* str){
    // Get better way to handle this.
    int len = strlen(str);
    if (len >= 99) return;

    // Normal typing
    if ((32 <= c && c <= 126)){
        str[len] = c;
        str[len+1] = '\0';
    }
    else if (c == 7){ // Return
        str[len-1] = '\0';
        str[len] = 0;
    }
}

void interpretChar2(char c, char* str, int* pos){
    // Get better way to handle this.
    int len = strlen(str);
    if (len >= 99) return;

    int x = *pos;

    // Normal typing
    if ((32 <= c && c <= 126)){
        str[x] = c;
        str[x+1] = '\0';
        x++;
    }
    else if (c == 7){ // Return
        str[x] = 0;
        x--;
    } else if (c == 4) { // Arrow Left
        x--;
    } else if (c == 5) { // Arrow Right
        x++;
    }

    *pos = x;
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
