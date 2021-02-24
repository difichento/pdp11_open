#ifndef PDP11_H
#define PDP11_H

#define pc reg[7]
#define MEM_SIZE 64 * 1024
#define BYTE_SIZE 8
#define REG_NUM 8

typedef unsigned char byte;
typedef unsigned short word;
typedef word Adress;

typedef struct {
    word mask;
    word opcode;
    char *name;

    void (*func)(void);
} Commands;

typedef struct {
    word val;
    word adr;
} Args;

void b_write(Adress adr, byte b);

byte b_read(Adress adr);

void w_write(Adress adr, word w);

word w_read(Adress adr);

void run();

Args get_mr(word w);

void print_reg();

void trace(char *format, ...);

void print_mem();

#endif