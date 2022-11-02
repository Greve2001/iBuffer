#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void testWindow();
void testBufferedWriting();
void interpretChar(char c, char* str);
void interpretChar2(char c, char* str, int* pos);

void tuiMain(){
    initscr(); cbreak(); noecho(); // Inital setup of screen
    keypad(stdscr, TRUE); // Enables navigation with keyboard

    //testWindow();
    testBufferedWriting();

    // End program
    getch(); 

    endwin();
}


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

void testBufferedWriting(){
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
        //printw("\t%d\t", c);
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