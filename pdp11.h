#ifndef PDP11_H
#define PDP11_H
#define pc reg[7]
typedef unsigned char byte;
typedef unsigned short word;
typedef word Adress;

void b_write(Adress adr, byte b);

byte b_read(Adress adr);

void w_write(Adress adr, word w);

word w_read(Adress adr);

void run();

typedef struct {
    word mask;
    word opcode;
    char *name;

    void (*func)(void);
} Commands;

#endif