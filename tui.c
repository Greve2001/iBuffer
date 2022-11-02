#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void testWindow();

void tuiMain(){
    initscr(); cbreak(); noecho(); // Inital setup of screen

    testWindow();

    // End program
    getch(); 
    endwin();
}


void testWindow(){
    WINDOW* testWin;

    // Create window
    char* str = "Why you not working!";
    int width = 50;
    int height = 30;

    testWin = newwin(height, width, 0, 0);

    // Append text
    refresh(); // VERY IMPORTANT! Dont know why
    mvwprintw(testWin, 1, 1, "%s", str);
    box(testWin, 0, 0);

    wrefresh(testWin); // Refresh

}