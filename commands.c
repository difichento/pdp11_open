#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
extern word reg[];
extern byte mem[];

void mov()
{
    pc+=2;
    printf("mov\n");
}

void halt()
{
    printf("halt\n");
    exit(0);
}

void add()
{
    printf("add\n");
}

void unknown()
{
    printf("unknown\n");
    exit(0);
}

Commands comms[] = {
        {0070000, 0010000, "mov",     mov},
        {0177777, 0000000, "halt",    halt},
        {0170000, 0060000, "add",     add},
        {0000000, 0000000, "unknown", unknown}
};

