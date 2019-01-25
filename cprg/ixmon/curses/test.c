#include <stdio.h>
#include <curses.h>

int main() {
    initscr();
    curs_set(FALSE); // hide cursor
    noecho();        // hide echo on keyboard press
    
    // int pos;
    // for (pos=0; pos<10; pos++) {
    //    clear();
    //    mvprintw(5,pos, "o");
    //    refresh();
    //    usleep(30000);
    // }
    
    // start_color();
    // attron(COLOR_PAIR(1));
    // init_pair(1, COLOR_RED, COLOR_BLACK);
    // mvprintw(5,5, "o");
    // attroff(COLOR_PAIR(1));
    // mvprintw(5,6, "o");

    int a=0;

    while (1) {
        if (a%4==1) mvprintw(5,5, "-");
        else if (a%4==2) mvprintw(5,5, "\\");
        else if (a%4==3) mvprintw(5,5, "|");
        else if (a%4==4) mvprintw(5,5, "/");
        a=a+1;
        usleep(100000);
        refresh();
    }
    
    
    getch();
    endwin();
    return 0;
}
