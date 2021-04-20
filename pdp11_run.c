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
            res.adr = reg[r];
            res.val = w_read(res.adr);      //  #nn
            reg[r] += 2;
            if (r == 7)
                trace("#%o ", res.val);
            else
                trace("(R%d) ", r);
            break;
        case 3:
            res.adr = reg[r];
            res.adr = w_read(res.adr);      //  @(Rn)+
            word tmp_for_trace = res.adr;
            res.val = w_read(res.adr);
            reg[r] += 2;
            if (r == 7)
                trace("@#%o ", tmp_for_trace);
            else
                trace("@(R%o)+ ", r);
            break;
        case 4:
            reg[r] -= 2;
            res.adr = reg[r];
            res.val = w_read(res.adr);      //  -(Rn)
            trace("-(R%o) ", r);
            break;
        case 5:
            reg[r] -= 2;
            res.adr = reg[r];
            res.adr = w_read(res.adr);      //  @-(Rn)
            res.val = w_read(res.adr);
            trace("@-(R%o) ", r);
            break;
        case 6: {
            word shift = w_read(pc);
            pc += 2;
            res.adr = reg[r] + shift;
            res.val = w_read(res.adr);
            if (r == 7)
                trace("%o ", shift);
            else
                trace("%o(R%o) ", shift, r);
            break;
        }
        case 7: {
            word shift = w_read(pc);
            pc += 2;
            res.adr = shift + reg[r];
            res.adr = w_read(res.adr);
            res.val = w_read(res.adr);
            if (r == 7)
                trace("@%o ", shift);
            else
                trace("@%o(R%o) ", shift, r);
            break;
        }

    }
    return res;
}


void run()
{
    trace("RUN:\n");
    pc = 01000;
    for (; pc < MEM_SIZE;) {
        word w = w_read(pc);
        pc += 2;
        trace("%06o %06o: ", pc, w);
        int i = 0;
        while (1) {
            Commands com = comms[i];
            if ((w & com.mask) == com.opcode) {
                trace(com.name);
                trace(" ");
                ss = get_mr(w >> 6);
                dd = get_mr(w);
                trace("\n");
                com.func();
                break;
            }
            i++;
        }
    }
}

