#include <ncurses.h>
#include <string.h>
#include <ctype.h>

#include "editor.h"
#include "misc.h"

void izvrsi_spremanje(Buffer *bf, Buffer *spr){
    //Ažuriraj ime datoteke
    bf->ime.duljina = spr->linije[0].duljina;

    bf->ime.podatci = (char *)malloc(bf->ime.duljina * sizeof(char));
    strcpy(bf->ime.podatci, spr->linije[0].podatci);
    
    //Ažuriraj traku
    uint32_t max_y = getmaxy(bf->prozor);
    move(max_y, 0);
    clrtoeol();
    move(0,0);

    FILE *f;
    f = fopen(bf->ime.podatci, "w");

    for(uint32_t i = 0; i < bf->broj_linija; i++){
        if(bf->linije[i].duljina > 0)
            fprintf(f, "%s", bf->linije[i].podatci);

        if(i != bf->broj_linija - 1)
            fprintf(f, "\n");
    }

    fclose(f);
}

bool spremi_operacije(Buffer *bf, Buffer *spr, int in){
    switch(in)
    {
    case KEY_DC:
        ocisti_redak(*spr);
        izbrisi_char(spr, 1);
        break;
    case KEY_BACKSPACE:
        ocisti_redak(*spr);
        izbrisi_char(spr, 0);
        break;
    case KEY_RIGHT:
        ocisti_redak(*spr);
        if(spr->kursor.stupac < spr->linije[0].duljina)
            spr->kursor.stupac++;
        break;
    case KEY_LEFT:
        ocisti_redak(*spr);
        if(spr->kursor.stupac > 0)
            spr->kursor.stupac--;
        break;
    default:
        if(isprint(in) || in == ' ')
            dodaj_char(spr, in);
        break;
    }

    switch(in)
    {
    case SPREMI:
    case 10: //ENTER
        izvrsi_spremanje(bf, spr);
    case IZLAZ:
        return true;
    default:
        break;
    }
    return false;
}

void spremi_print(Buffer spr, uint32_t y, uint32_t x){
    box(spr.prozor, 0, 0);
    mvwprintw(spr.prozor, 0, x/2 - 3, "SPREMI");
    mvwprintw(spr.prozor, y-1, 2, "ENTER = OK");
    mvwprintw(spr.prozor, y-1, x - 16, "CTRL-X = IZLAZ");

    mvwprintw(spr.prozor, 1, 2, "Spremi datoteku kao:");
    mvwprintw(spr.prozor, 2, 2, spr.linije->podatci);

    mvwchgat(spr.prozor, 2, spr.kursor.stupac + 2, 1, A_STANDOUT | A_BLINK, COLOR_PAIR(1), NULL);
    
    wrefresh(spr.prozor);
}

void spremi(Buffer *bf){
    int in;
    bool exit = false;

    uint32_t max_y, max_x;
    getmaxyx(bf->prozor, max_y, max_x);

    uint32_t y = 5, x = 40;
    WINDOW *win = newwin(y, x, max_y/2 - y/2, max_x/2 - x/2);
    Linija linije[1] = {{bf->ime.podatci, bf->ime.duljina}};
    Buffer spr = {{bf->ime.duljina, 0}, max_y/2 - y/2 + 2, linije, 1, win, {"SPREMI", 6}};

    while(!exit){
        spremi_print(spr, y, x);
        in = getch();
        exit = spremi_operacije(bf, &spr, in);
    }
}