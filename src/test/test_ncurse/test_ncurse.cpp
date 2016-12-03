//
// Created by root on 10/23/16.
//

#include <iostream>
#include <ncurses.h>
#include <string.h>


int main()
{
    int ch;
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    //output
    printw("Type any character\n");
    ch = getch();

    if (ch == KEY_F(2)) {
        printw("F1 key pressed!");
    } else {
        printw("the key you pressed is!");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    }

    //input
    char mesg[] = "Just a string";
    int row, col;
    getmaxyx(stdscr, row, col);
    mvprintw(row/2, (col - strlen(mesg))/2, "%s", mesg);

    char readstr[100];
    getstr(readstr);
    attron(A_REVERSE | A_BLINK);
    mvprintw(row/2 + 1, (col - strlen(mesg))/2, "%s", readstr);
    attroff(A_REVERSE | A_BLINK);

    addch('C' | A_STANDOUT);

    refresh();
    getch();
    endwin();
}

