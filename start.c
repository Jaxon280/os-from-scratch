#include "int.h"

#define MPP_MMASK 0b11 << 11
#define MIE_MASK (1 << 3) | (1 << 7)

__attribute__((aligned(16))) char _stack[4096]; // aligned here because stack pointer is always kept 16-byte alignment in RISC-V

void main();

static inline void set_mstatus(uint64 *x)
{
    *x |= MIE_MASK;
    *x &= ~MPP_MMASK;
    *x |= (0b1 << 11); // set MPP to use Supervisor mode.
}

static inline void w_mstatus(uint64 x)
{
    asm volatile("csrw mstatus, %0"
                 :
                 : "r"(x));
}

static inline void w_mepc(uint64 x)
{
    asm volatile("csrw mepc, %0"
                 :
                 : "r"(x)); // set address retured from mret
}

static inline void w_satp(uint64 x)
{
    asm volatile("csrw satp, %0"
                 :
                 : "r"(x)); // clear satp(pgdir address register) here.
}

static inline void w_medeleg(uint64 x)
{
    asm volatile("csrw medeleg, %0"
                 :
                 : "r"(x)); // delegation to supervisor mode.
}

static inline void w_mideleg(uint64 x)
{
    asm volatile("csrw mideleg, %0"
                 :
                 : "r"(x)); // delegation to supervisor mode.
}

void start()
{
    uint64 mstat = 0;
    set_mstatus(&mstat);

    w_mstatus(mstat);

    w_mepc((uint64)main);

    w_satp(0);

    w_medeleg(0x0001);
    w_mideleg(0x0001);

    asm volatile("mret");
}
