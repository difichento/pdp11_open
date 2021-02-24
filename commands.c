#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>

extern word reg[];
extern byte mem[];
extern Args ss, dd;

void mov()
{
    trace("mov\n");
    if (dd.adr < 8)
        reg[dd.adr] = ss.val;
    else
        w_write(dd.adr, ss.val);
}

void halt()
{
    trace("halt\n\n");
    print_reg();
    exit(0);
}

void add()
{
    trace("add\n");
    if (dd.adr < 8)
        reg[dd.adr] += ss.val;
    else
        w_write(dd.adr, w_read(dd.adr) + ss.val);
}

void unknown()
{
    trace("unknown\n");
}


Commands comms[] = {
        {0070000, 0010000, "mov",     mov},
        {0177777, 0000000, "halt",    halt},
        {0170000, 0060000, "add",     add},
        {0000000, 0000000, "unknown", unknown}
};

