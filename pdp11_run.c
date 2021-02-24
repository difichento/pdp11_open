#include "pdp11.h"
#include <stdio.h>

extern word reg[];
extern byte mem[];
extern Commands comms[];
extern Args ss, dd;

Args get_mr(word w)     // get mode && arg
{
    Args res;
    int r = w & 7;      // reg number
    int mode = (w >> 3) & 7;
    //printf("\n\nGET:\ninput: %o\nr: %o\n", w, r);
    //printf("mode: %o\n", mode);
    switch (mode) {
        case 0:             // Rn
            res.adr = r;
            res.val = reg[r];
            trace("R%o ", r);
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
                trace("#%o ", res.val);
                break;
            } else {
                res.adr = reg[r];
                res.val = w_read(res.adr);      //  (Rn)+
                trace("(R%o)+ ", r);
                reg[r] += 2;
                break;
            }
        case 3:
            if (r == 7) {

                res.adr = reg[r];
                res.val = w_read(res.adr + 2);      //  @#nn
                res.val = w_read(res.val);
                trace("@#%o ", res.val);
                reg[r] += 2;
                break;
            } else {
                res.adr = reg[r];
                res.val = w_read(res.adr);      //  @(Rn)+
                res.val = w_read(res.val);
                trace("@(R%o)+ ", r);
                reg[r] += 2;
                break;
            }
        case 4:
            res.adr = reg[r] - 2;
            res.val = w_read(res.adr);      //  -(Rn)
            trace("-(R%o)+ ", r);
            break;
        case 5:
            res.adr = reg[r] - 2;
            res.val = w_read(res.adr);      //  @-(Rn)
            res.val = w_read(res.val);
            trace("@-(R%o)+ ", r);
            break;
    }
    return res;
}

void print_mem()
{
    trace("MEM:\n");
    int a = 01000;
    int b = w_read(a);
    while (b != 0) {
        trace("%06o\n", b);
        a += 2;
        b = w_read(a);
    }
    trace("\n");
}

void run()
{
    pc = 01000;

    for (; pc < MEM_SIZE; pc += 2) {
        word w = w_read(pc);
        trace("%06o %06o ", pc, w);
        int i = 0;
        while (1) {
            Commands com = comms[i];
            if ((w & com.mask) == com.opcode) {
                ss = get_mr(w >> 6);
                dd = get_mr(w);
                com.func();
                break;
            }
            i++;
        }
    }
}

