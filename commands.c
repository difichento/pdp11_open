#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>

extern word reg[];
extern byte mem[];
extern Args ss, dd;

void mov()
{
    ss = get_mr(w_read(pc) >> 6);
    dd = get_mr(w_read(pc - 2));
    printf("dd.adr = %o\nss.val = %o\n", dd.adr, ss.val);
    reg[dd.adr] = ss.val;
    printf("\n");
}

void halt()
{
    printf("\nhalt\n");
    print_reg();
    exit(0);
}

void add()
{
    ss = get_mr(w_read(pc) >> 6);
    dd = get_mr(w_read(pc));
    printf("ADD:\n%o + %o(R%d) = %o\n", ss.val, dd.val, dd.adr, reg[dd.adr] + ss.val);
    reg[dd.adr] += ss.val;

}

void unknown()
{
    printf("%o: unknown\n", pc);
}


Commands comms[] = {
        {0070000, 0010000, "mov",     mov},
        {0177777, 0000000, "halt",    halt},
        {0170000, 0060000, "add",     add},
        {0000000, 0000000, "unknown", unknown}
};

