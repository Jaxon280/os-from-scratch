#include "asm.h"
#include "type.h"

__attribute__((aligned(16))) char _stack[4096]; // aligned here because stack pointer is always kept 16-byte alignment in RISC-V

void main();

void start()
{
    uint64 mstat;
    set_mstatus(&mstat);
    w_mstatus(mstat);

    w_mepc((uint64)main);

    w_satp(0); // disable address translation (use bare mode)

    w_medeleg(0xffff);
    w_mideleg(0xffff);
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

    asm volatile("mret");
}
