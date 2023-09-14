#include <ncurses.h>
#include <stdlib.h>

#include "editor.h"

void ocisti_redak(Buffer bf){
    wmove(bf.prozor, bf.kursor.br_linije - bf.kamera + 1, bf.kursor.stupac);
    wclrtoeol(bf.prozor);
}

void ocisti_sve_do(Buffer bf, uint32_t x){
    for(uint32_t i = 0; i < x; i++){
        wmove(bf.prozor, i, 0);
        wclrtoeol(bf.prozor);
    }
}

void traka_print(Buffer bf, uint32_t max_y, uint32_t max_x){
    init_pair(1, COLOR_RED, COLOR_BLACK);
    mvprintw(max_y - 1, 10, "[%s] \t (%d, %d) \t broj linija: %d", 
        bf.ime.podatci, bf.kursor.br_linije + 1, bf.kursor.stupac, bf.broj_linija);
    mvchgat(max_y - 1, 0, max_x, A_STANDOUT, COLOR_PAIR(1), NULL);
}