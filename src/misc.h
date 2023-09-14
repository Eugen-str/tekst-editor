#ifndef MISC_H_
#define MISC_H_

#include "editor.h"

typedef enum{
    SPREMI = 's' & 0x1F,
    IZLAZ = 'x' & 0x1F,
}Komanda;

void ocisti_redak(Buffer bf);
void ocisti_sve_do(Buffer bf, uint32_t x);

void traka_print(Buffer bf, uint32_t max_y, uint32_t max_x);
void box_print(Buffer bf);
#endif // MISC_H_