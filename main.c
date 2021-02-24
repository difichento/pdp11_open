#include <stdio.h>
#include <assert.h>
#include "pdp11.h"
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

byte mem[MEM_SIZE];
word reg[8];
Args ss, dd;

void mem_test()
{
    Adress adr0 = 2;

    byte b0 = 0x0f;
    byte b1 = 0xab;
    word w0 = 0xab0f;
    b_write(adr0, b0);
    byte bres0 = b_read(adr0);      // test 1, bw/br
    //trace("t1:\n%02hhx = %02hhx = %02hhx\n", b0, mem[adr0], bres0);     // test 1
    assert(bres0 == b0);

    b_write(adr0 + 1, b1);
    word wres0 = w_read(adr0);      // test 2, bw/bw/wr
    //trace("t2:\n%02hhx%02hhx = %02hhx%02hhx = %04hx\n", b1, b0, mem[adr0 + 1], mem[adr0], wres0);
    assert(wres0 == w0);

    Adress adr1 = 4;
    word w1 = 0x05a0;
    byte b2 = 0x05;
    byte b3 = 0xa0;

    w_write(adr1, w1);
    word wres1 = w_read(adr1);      // test 3, ww/wr
    //trace("t3:\n%04hx = %02hhx%02hhx = %04hx\n", w1, mem[adr1 + 1], mem[adr1], wres1);
    assert(w1 == wres1);

    byte b4 = b_read(adr1 + 1);
    byte b5 = b_read(adr1);         // test 4, ww/br
    //trace("t3:\n%04hx = %02hhx%02hhx = %02hhx%02hhx\n", w1, b2, b3, b4, b5);
    assert(b4 == b2 && b5 == b3);
}

void load_file(const char *filename)
{
    FILE *fin = fopen(filename, "r");
    if (fin == NULL) {
        perror(filename);
        exit(errno);
    }

    Adress st_adr;
    word N;
    while (fscanf(fin, "%hx", &st_adr) != EOF) {
        fscanf(fin, "%hx", &N);
        //trace("start adress: %o\n", st_adr);
        //trace("N: %d\n", N);
        for (int i = 0; i < N; ++i) {
            byte temp;
            fscanf(fin, "%hhx", &temp);
            b_write(st_adr + i, temp);
            //trace("Written %02hhx to %o\n", temp, st_adr + i);
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    load_file("tests/test2/test.o");

    print_mem();
    trace("\n");
    run();
    return 0;
}

void b_write(Adress adr, byte b)
{
    mem[adr] = b;
}

byte b_read(Adress adr)
{
    return mem[adr];
}

void w_write(Adress adr, word w)
{
    mem[adr] = (byte) w;
    mem[adr + 1] = (byte) (w >> BYTE_SIZE);
}

word w_read(Adress adr)
{
    return mem[adr] | ((word) mem[adr + 1] << BYTE_SIZE);
}

void print_reg()
{
    for (int i = 0; i < REG_NUM; ++i) {
        printf("reg[%d]: %06o\n", i, reg[i]);
    }
}


void trace(char *format, ...)
{
    va_list ptr;
    va_start(ptr, format);
    vprintf(format, ptr);
    va_end(ptr);
}
