#include "pdp11.h"
#include <stdio.h>

extern word reg[];
extern byte mem[];
extern Commands comms[];

Args get_mr(word w)     // get mode && arg
{

    Args res;
    int r = w & 7;      // reg number
    int mode = (w >> 3) & 7;
    printf("\n\nGET:\ninput: %o\nr: %o\n", w, r);
    printf("mode: %o\n", mode);
    switch (mode) {
        case 0:             // Rn
            res.adr = r;
            res.val = reg[r];
            printf("R%o\n", r);
            break;
        case 1:             //  (Rn)
            res.adr = reg[r];
            res.val = w_read(res.adr);
            break;
        case 2:
            if (r == 7) {
                reg[r] += 2;
                res.adr = reg[r];
                res.val = w_read(res.adr);      //  #nn
                printf("#%o\n", res.val);
                break;
            } else {
                res.adr = reg[r];
                res.val = w_read(res.adr);      //  (Rn)+
                printf("(R%o)+\n", r);
                reg[r] += 2;
                break;
            }
        case 3:
            if (r == 7) {
                reg[r] += 2;
                res.adr = reg[r];
                res.val = w_read(res.adr);      //  @#nn
                res.val = w_read(res.val);
                printf("@#%o\n", res.val);
                break;
            } else {
                res.adr = reg[r];
                res.val = w_read(res.adr);      //  @(Rn)+
                res.val = w_read(res.val);
                printf("@(R%o)+\n", r);
                reg[r] += 2;
                break;
            }
        case 4:
            res.adr = reg[r] - 2;
            res.val = w_read(res.adr);      //  -(Rn)
            printf("-(R%o)+\n", r);
            break;
        case 5:
            res.adr = reg[r] - 2;
            res.val = w_read(res.adr);      //  @-(Rn)
            res.val = w_read(res.val);
            printf("@-(R%o)+\n", r);
            break;
    }
    printf("\n\n");
    return res;
}

void print_mem()
{
    int a = 01000;
    int b = w_read(a);
    while (b != 0) {
        printf("%o\n", b);
        a += 2;
        b = w_read(a);
    }
}

void run()
{
    pc = 01000;
    int i = 0;
    print_mem();
    while (1) {
        word w = w_read(pc);
        printf("read %o: %06o \n", pc, w);
        i = 0;
        while (1) {
            Commands com = comms[i];
            if ((w & com.mask) == com.opcode) {
                com.func();
                break;
            }
            i++;
        }
        pc += 2;
    }

}

