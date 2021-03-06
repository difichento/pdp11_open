#include "pdp11.h"
#include <stdio.h>
#include <string.h>

extern word reg[];
extern byte mem[];
extern Commands comms[];
extern Args ss, dd, nn, r, b_flag, xx;

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
            if (b_flag.val && r != 7 && r != 6)
                reg[r] += 1;
            else
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
            if (b_flag.val && r != 7 && r != 6)
                reg[r] -= 1;
            else
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

void get_r(word w, Commands com)
{
    if (!strcmp(com.name, "rts"))
        r.val = w & 7;
    else
        r.val = (w >> 6) & 07;
}

void get_b(word w)
{
    b_flag.val = (w >> 15) & 7;
}

void get_xx(word w)
{
    xx.val = w & 0377;
}

int need_ss(Commands com)
{
    return (com.params & 7);
}

int need_dd(Commands com)
{
    return ((com.params >> 3) & 1);
}

int need_b(Commands com)
{
    return ((com.params >> 6) & 1);
}

int need_nn(Commands com)
{
    return ((com.params >> 9) & 1);
}

int need_r(Commands com)
{
    return ((com.params >> 12) & 1);
}

int need_xx(Commands com)
{
    return ((com.params >> 15) & 1);
}

void run()
{
    trace("----------run----------\n");
    pc = 01000;
    w_write(ostat, 0x0000 + 0x80);
    while (1) {
        word w = w_read(pc);
        trace("%06o %06o: ", pc, w);
        pc += 2;
        int i = 0;
        while (1) {
            Commands com = comms[i];
            if ((w & com.mask) == com.opcode) {
                trace(com.name);
                if (need_b(com)) {
                    get_b(w);
                    if (b_flag.val)
                        trace("b");
                }
                trace(" ");
                if (need_ss(com)) {
                    ss = get_ss_dd(w >> 6);
                }
                if (need_dd(com)) {
                    dd = get_ss_dd(w);
                }

                if (need_r(com))
                    get_r(w, com);
                if (need_nn(com))
                    get_nn(w);
                if (need_xx(com))
                    get_xx(w);

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

