#include "pdp11.h"
#include <stdio.h>

extern word reg[];
extern byte mem[];
extern Commands comms[];
extern Args ss, dd, nn, r, B;

Args get_ss_dd(word w)     // get mode && arg
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
            trace("(R%d) ", r);
            break;
        case 2:
            res.adr = reg[r];
            res.val = w_read(res.adr);      //  #nn
            reg[r] += 2;
            if (r == 7)
                trace("#%o ", res.val);
            else
                trace("(R%d)+ ", r);
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

void get_nn(word w)
{
    nn.val = w & 077;
}

void get_r(word w)
{
    r.val = (w >> 6) & 07;
}

void get_b(word w)
{
    B.val = (w >> 15) & 7;
}

int need_ss(Commands com)
{
    return (com.params & 07) == 1;
}

int need_dd(Commands com)
{
    return ((com.params >> 3) & 7) == 1;
}

int need_b(Commands com)
{
    return ((com.params >> 6) & 7) == 1;
}

int need_nn(Commands com)
{
    return ((com.params >> 9) & 7) == 1;
}

int need_r(Commands com)
{
    return ((com.params >> 12) & 7) == 1;
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
                if (need_ss(com))
                    ss = get_ss_dd(w >> 6);
                if (need_dd(com))
                    dd = get_ss_dd(w);
                if (need_b(com))
                    get_b(w);
                if (need_r(com))
                    get_r(w);
                if (need_nn(com))
                    get_nn(w);

                com.func();
                trace("\n");
                print_reg();
                trace("\n");
                break;
            }
            i++;
        }
    }
}

