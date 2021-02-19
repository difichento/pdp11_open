#include "pdp11.h"
#include <stdio.h>

extern word reg[];
extern byte mem[];
extern Commands comms[];

void run()
{
    pc = 01000;
    int i = 0;
    while (1) {
        word w = w_read(pc);
        //printf("read: %06o --- ", w);
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