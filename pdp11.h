#ifndef PDP11_H
#define PDP11_H

typedef unsigned char byte;
typedef unsigned short word;
typedef word Adress;

void b_write(Adress adr, byte b);

byte b_read(Adress adr);

void w_write(Adress adr, word b);

word w_read(Adress adr);

#endif