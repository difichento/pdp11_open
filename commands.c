#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>

extern word reg[];
extern byte mem[];
extern Args ss, dd, b_flag, r, nn, xx;
extern char N_flag, Z_flag, V_flag, C_flag;

void set_NZ(word value)
{
    if (value == 0) {
        N_flag = 0;
        Z_flag = 1;
    } else if ((value >> 15) & 1) {
        N_flag = 1;
        Z_flag = 0;
    } else if (value > 0) {
        N_flag = 0;
        Z_flag = 0;
    }
}

void set_ALL(word val2, int c)
{
    set_NZ(val2);
    V_flag = 0;
    C_flag = c;

    /*if (((val1 >> 14) & 1) == 1 && ((val2 >> 14) & 1) == 0 && ((val2 >> 15) != (val1 >> 15)))
        V_flag = 1;*/
}

void mov()
{
    if (b_flag.val) {
        b_write(dd.adr, (byte) ss.val);
        trace("\t[%06o] = %06o", ss.adr, (byte) ss.val);
    } else {
        w_write(dd.adr, ss.val);
        trace("\t[%06o] = %06o", ss.adr, ss.val);
    }
    set_NZ(w_read(dd.adr));
    if (dd.adr == odata)
        printf("%c", w_read(odata));


}

void halt()
{
    printf("\n----------halted----------\n");
    print_reg_halted();
    exit(0);
}

void add()
{
    set_ALL(w_read(dd.adr) + ss.val, ((w_read(dd.adr) + ss.val) >> 16) & 1);
    w_write(dd.adr, (word) (w_read(dd.adr) + ss.val));
    if (dd.adr < 8 && ss.adr < 8)
        trace("\t\tR%o = %o; R%o = %o", ss.adr, ss.val, dd.adr, w_read(dd.adr));
    else if (ss.adr >= 8 && dd.adr < 8)
        trace("\t\t[%06o] = %06o; R%o = %o", ss.adr, ss.val, dd.adr, w_read(dd.adr));
    else if (dd.adr >= 8 && ss.adr >= 8)
        trace("\t\t[%06o] = %06o; [%06o] = %06o", ss.adr, ss.val, dd.adr, w_read(dd.adr));
    else
        trace("\t\tR%d = %o; [%06o] = %06o", ss.adr, ss.val, dd.adr, w_read(dd.adr));
    // trace("\nNZVC = %d%d%d%d", N_flag, Z_flag, V_flag, C_flag);
}

void clr()
{
    if (b_flag.val)
        b_write(dd.adr, 0);
    else
        w_write(dd.adr, 0);
    set_ALL(0, 0);

}

void sob()
{
    reg[r.val] -= 1;
    if (reg[r.val] != 0) {
        pc -= nn.val * 2;

    }
}

void cln()
{
    N_flag = 0;
}

void clz()
{
    Z_flag = 0;
}

void clv()
{
    V_flag = 0;
}

void clc()
{
    C_flag = 0;
}

void ccc()
{
    N_flag = 0;
    Z_flag = 0;
    V_flag = 0;
    C_flag = 0;
}

void sen()
{
    N_flag = 1;
}

void sez()
{
    Z_flag = 1;
}

void sev()
{
    V_flag = 1;
}

void sec()
{
    C_flag = 1;
}

void scc()
{
    N_flag = 1;
    Z_flag = 1;
    V_flag = 1;
    C_flag = 1;
}

void br()
{
    pc += (char) xx.val * 2;
    trace("\t\t pc -= %d", (char) xx.val * 2);
}

void beq()
{
    if (Z_flag == 1)
        br();
}

void bne()
{
    if (Z_flag == 0)
        br();
}

void bmi()
{
    if (N_flag == 1)
        br();
}

void bpl()
{
    if (N_flag == 0)
        br();
}

/*
void blt()
{
    if ((N_flag ^ V_flag) == 1)
        br();
}

void bge()
{
    if ((N_flag ^ V_flag) == 0)
        br();
}

void ble()
{
    if ((Z_flag | (N_flag ^ V_flag)) == 1)
        br();
}
*/

void tst()
{
    ccc();
    if (b_flag.val) {
        byte tmp = b_read(dd.adr);
        if (sign(tmp))
            set_NZ(0xff00 + tmp);
        else
            set_NZ(0x0000 + tmp);
    } else {
        set_NZ(w_read(dd.adr));
    }
}

void jsr()
{
    word tmp = dd.adr;
    sp -= 2;
    w_write(sp, reg[r.val]);
    reg[r.val] = pc;
    pc = tmp;
    trace(" %06o", reg[r.val]);
}

void rts()
{
    pc = reg[r.val];
    reg[r.val] = w_read(sp);
    sp += 2;
    trace("\tr%d: %06o", r.val, reg[r.val]);
}

void inc()
{
    w_write(dd.adr, dd.val + 1);
    set_NZ(dd.val + 1);
}

void rol()
{
    int w = dd.val;
    w = w << 1;
    w += C_flag;
    set_ALL((word) w, (w >> 16) & 1);
    w_write(dd.adr, (word) w);
}

void unknown()
{
    printf("unknown command\n");
    exit(1);
}

// params: 0XRNBDS
Commands comms[] = {
        {0070000, 0010000, "mov",     mov,     0000111},
        {0177777, 0000000, "halt",    halt,    0000000},
        {0170000, 0060000, "add",     add,     0000011},
        {0077700, 0005000, "clr",     clr,     0000110},
        {0177000, 0077000, "sob",     sob,     0011000},
        {0177777, 0000250, "cln",     cln,     0000000},
        {0177777, 0000244, "clz",     clz,     0000000},
        {0177777, 0000242, "clv",     clv,     0000000},
        {0177777, 0000241, "clc",     clc,     0000000},
        {0177777, 000257,  "ccc",     ccc,     0000000},
        {0177777, 0000250, "sen",     sen,     0000000},
        {0177777, 0000244, "sez",     sez,     0000000},
        {0177777, 0000242, "sev",     sev,     0000000},
        {0177777, 0000241, "sec",     sec,     0000000},
        {0177777, 0000257, "scc",     scc,     0000000},
        {0177400, 0000400, "br",      br,      0100000},
        {0177400, 0001400, "beq",     beq,     0100000},
        {0177400, 0001000, "bne",     bne,     0100000},
        {0177400, 0100400, "bmi",     bmi,     0100000},
        {0177400, 0100000, "bpl",     bpl,     0100000},
        /*{0177400, 0002400, "blt",     blt,     0100000},
        {0177400, 0002000, "bge",     bge,     0100000},
        {0177400, 0003400, "ble",     ble,     0100000},*/
        {0007700, 0005700, "tst",     tst,     0000110},
        {0177000, 0004000, "jsr",     jsr,     0010010},
        {0177770, 0000200, "rts",     rts,     0010000},
        {0077700, 0005200, "inc",     inc,     0000110},
        {0077700, 0006100, "rol",     rol,     0000110},
        {0000000, 0000000, "unknown", unknown, 0000000}
};

