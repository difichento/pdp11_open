#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>

extern word reg[];
extern byte mem[];
extern Args ss, dd, B, r, nn;

void mov()
{
    w_write(dd.adr, ss.val);
    trace("\t\t[%06o] = %06o", ss.adr, ss.val);
}

void halt()
{
    trace("\nREG:\n");
    print_reg();
    exit(0);
}

void add()
{
    w_write(dd.adr, w_read(dd.adr) + ss.val);
    trace("\t\tR%d = %o; R%d = %o", dd.adr, dd.val, ss.adr, ss.val);
}

void clr()
{
    w_write(dd.adr, 0);
}

void sob()
{
    reg[r.val] -= 1;
    if (reg[r.val] != 0) {
        pc -= nn.val * 2;

    }
}

void unknown()
{
}

// params: 00RNBDS
Commands comms[] = {
        {0070000, 0010000, "mov",     mov,     0000111},
        {0177777, 0000000, "halt",    halt,    0000000},
        {0170000, 0060000, "add",     add,     0000011},
        {0077700, 0005000, "clr",     clr,     0000110},
        {0177000, 0077000, "sob",     sob,     0011000},
        {0000000, 0000000, "unknown", unknown, 0000000}
};

