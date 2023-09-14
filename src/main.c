#include <ncurses.h>

#include "editor.h"

int main(){
    initscr();
    noecho();
    raw();
    curs_set(0);
    keypad(stdscr, TRUE);

    uint32_t max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *win = newwin(max_y - 1, max_x - 5, 0, 5);
    Kursor kursor = {0, 0};
    Linija linije[MAX_LINIJA] = {{'\0', 0}};
    Buffer bf = {kursor, 0, linije, 1, win, {"nova datoteka", 13}};

    bool exit = false;
    int in;

    while(!exit){
        getmaxyx(stdscr, max_y, max_x);
        buffer_print(bf, max_y, max_x);
        in = getch();
        //printw("%d", in);
        //getch();
        exit = buffer_update(&bf, in, max_y);
    }
    endwin();
    return 0;
}