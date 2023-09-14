#ifndef EDITOR_H_
#define EDITOR_H_

#define MAX_LINIJA 256

#include <stdlib.h>

typedef struct{
    char *podatci;
    uint32_t duljina;
}Linija;

typedef struct{
    uint32_t stupac;
    uint32_t br_linije;
}Kursor;

typedef struct{
    Kursor kursor;
    uint32_t kamera;
    Linija *linije;
    uint32_t broj_linija;
    WINDOW *prozor;
    Linija ime;
}Buffer;

void buffer_print(Buffer bf, uint32_t max_y, uint32_t max_x);
bool buffer_update(Buffer *bf, int in, uint32_t max_y);

void dodaj_char(Buffer *bf, char c);
void izbrisi_char(Buffer *bf, int smjer);
#endif //EDITOR_H_