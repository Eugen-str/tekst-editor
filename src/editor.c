#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

#include "editor.h"
#include "misc.h"
#include "spremi.h"

void buffer_print(Buffer bf, uint32_t max_y, uint32_t max_x){
    WINDOW *brojevi = newwin(max_y - 1, 5, 0, 0);
    box(brojevi, 0, 0);
    box(bf.prozor, 0, 0);

    for(uint32_t i = bf.kamera; i < bf.broj_linija && i < bf.kamera + max_y - 3; i++){
        //Pisanje brojeva linija
        wmove(brojevi, i - bf.kamera + 1, 1);
        wprintw(brojevi, "%3d", i + bf.kamera + 1);

        wmove(bf.prozor, i - bf.kamera + 1, 1);
        if(bf.linije[i].duljina > 0)
            wprintw(bf.prozor, "%s", bf.linije[i].podatci);
    }
    //Crtanje kursora
    mvwchgat(bf.prozor, bf.kursor.br_linije - bf.kamera + 1, bf.kursor.stupac + 1, 1, A_STANDOUT | A_BLINK, COLOR_PAIR(5), NULL);

    traka_print(bf, max_y, max_x);
    wmove(bf.prozor, 0, 0);

    wrefresh(bf.prozor);
    wrefresh(brojevi);
}

void dodaj_char(Buffer *bf, char c){
    uint32_t linija = bf->kursor.br_linije;
    uint32_t trenutni_polozaj = bf->kursor.stupac;
    uint32_t duljina_linije = bf->linije[linija].duljina;

    size_t j = 0;
    char temp[duljina_linije + 1];
    
    if(duljina_linije == 0){
        temp[0] = c;
        temp[1] = '\0';

        j++;
    }
    else{
        for(uint32_t i = 0; i <= duljina_linije; i++, j++){
            if(i == trenutni_polozaj){
                temp[j] = c;
                j++;
            }
            temp[j] = bf->linije[linija].podatci[i];
        }
    }

    bf->linije[linija].duljina++;
    temp[bf->linije[linija].duljina] = '\0';

    //Makni stare podatke i alociraj mjesto za nove
    //free(bf->linije[linija].podatci);
    bf->linije[linija].podatci = (char *)malloc(j * sizeof(char));
    
    strcpy(bf->linije[linija].podatci, temp);

    //Pomakni kursor za jedan u desno
    bf->kursor.stupac++;
}

void izbrisi_char(Buffer *bf, int smjer){
    uint32_t linija = bf->kursor.br_linije;
    uint32_t trenutni_polozaj = bf->kursor.stupac;
    uint32_t duljina_linije = bf->linije[linija].duljina;

    if(duljina_linije == 0 || (trenutni_polozaj == 0 && smjer == 0) || (trenutni_polozaj == duljina_linije && smjer == 1)){
        return;
    }

    char temp[duljina_linije - 1];
    uint32_t j = 0;
    for(uint32_t i = 0; i <= duljina_linije; i++, j++){
        if(i == trenutni_polozaj - 1 + smjer)
            j++;
        temp[i] = bf->linije[linija].podatci[j];
    }

    temp[bf->linije[linija].duljina] = '\0';
    bf->linije[linija].duljina--;

    //Izbrisi stare podatke i alociraj mjesto za nove
    //free(bf->linije[linija].podatci);
    bf->linije[linija].podatci = (char *)malloc(j * sizeof(char));
    
    strcpy(bf->linije[linija].podatci, temp);

    //Pomakni kursor za jedan u lijevo
    if(smjer == 0)
        bf->kursor.stupac--;
}

void izvrsi_enter(Buffer *bf){
    if(bf->broj_linija - 1 == bf->kursor.br_linije){
        bf->broj_linija++;
        bf->kursor.br_linije++;
        bf->kursor.stupac = 0;
        bf->linije[bf->kursor.br_linije].duljina = 0;
        //bf->linije[bf->kursor.br_linije].podatci = (char *)malloc(sizeof(char));
        bf->linije[bf->kursor.br_linije].podatci = '\0';
    }/*
    else{
        char *preostalo;
        preostalo = (char *)malloc(bf->linije[bf->kursor.br_linije].duljina - bf->kursor.stupac * sizeof(char));
        
        uint32_t j = 0;
        uint32_t i = bf->kursor.stupac;
        while(i < bf->linije[bf->kursor.br_linije].duljina - bf->kursor.stupac){
            preostalo[j] = bf->linije[bf->kursor.br_linije].podatci[i];
            i++; j++;
        }

        bf->linije[bf->broj_linija].podatci = (char *)malloc(bf->linije[bf->broj_linija - 1].duljina * sizeof(char));
        for(uint32_t i = bf->broj_linija; i > bf->kursor.br_linije; i++){
            strcpy(bf->linije[i].podatci, bf->linije[i - 1].podatci);
            bf->linije[i].duljina = bf->linije[i - 1].duljina;
        }

        strcpy(bf->linije[bf->kursor.br_linije].podatci, preostalo);

        bf->broj_linija++;
        bf->kursor.br_linije++;
        bf->kursor.stupac = 0;
    }*/
}

bool buffer_update(Buffer *bf, int in, uint32_t max_y){
    // standardan input
    switch(in)
    {
    case KEY_LEFT:
        ocisti_redak(*bf);
        if(bf->kursor.stupac > 0){
            bf->kursor.stupac--;
        }
        else if(bf->kursor.br_linije > 0 && bf->kursor.stupac == 0){
            bf->kursor.stupac = bf->linije[bf->kursor.br_linije - 1].duljina;
            bf->kursor.br_linije--;
        }
        break;
    case KEY_RIGHT:
        ocisti_redak(*bf);
        if(bf->kursor.stupac == bf->linije[bf->kursor.br_linije].duljina && bf->broj_linija - 1 > bf->kursor.br_linije){
            bf->kursor.stupac = 0;
            bf->kursor.br_linije++;
        }
        else if(bf->kursor.stupac < bf->linije[bf->kursor.br_linije].duljina)
            bf->kursor.stupac++;
        break;
    case KEY_UP:
        ocisti_redak(*bf);
        if(bf->kursor.br_linije > 0){
            if(bf->kursor.stupac > bf->linije[bf->kursor.br_linije - 1].duljina)
                bf->kursor.stupac = bf->linije[bf->kursor.br_linije - 1].duljina;
            bf->kursor.br_linije--;
        }
        if(bf->kursor.br_linije == bf->kamera + 1 && bf->kamera > 0){
            bf->kamera--;
            ocisti_sve_do(*bf, max_y - 1);
        }
        break;
    case KEY_DOWN:
        ocisti_redak(*bf);
        if(bf->kursor.br_linije < bf->broj_linija - 1){
            if(bf->kursor.stupac > bf->linije[bf->kursor.br_linije + 1].duljina)
                bf->kursor.stupac = bf->linije[bf->kursor.br_linije + 1].duljina;
            
            bf->kursor.br_linije++;
        }
        if(bf->kursor.br_linije == max_y - 3 + bf->kamera){
            bf->kamera++;
            ocisti_sve_do(*bf, max_y - 1);
        }
        break;
    case 10: // ENTER
        ocisti_redak(*bf);
        izvrsi_enter(bf);
        if(bf->kursor.br_linije == max_y - 3 + bf->kamera){
            bf->kamera += 1;
            ocisti_sve_do(*bf, max_y - 1);
        }
        break;
    case '\t':
        for(uint32_t i = 0; i < 4; i++){
            dodaj_char(bf, ' ');
        }
        break;
    case KEY_DC:
        ocisti_redak(*bf);
        izbrisi_char(bf, 1);
        break;
    case KEY_BACKSPACE:
        ocisti_redak(*bf);
        izbrisi_char(bf, 0);
        break;
    default:
        if(isprint(in) || in == ' '){
            dodaj_char(bf, in);
        }
        break;
    }
    
    // komande
    switch (in)
    {
    case SPREMI:
        spremi(bf);
        break;
    case IZLAZ:
        return true;
        break;
    default:
        break;
    }
    return false;
}