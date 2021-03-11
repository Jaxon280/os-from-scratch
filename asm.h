#include "type.h"

#define MPP_MMASK (3L << 11)
#define MPP_S (1L << 11)
#define MIE_MASK (1 << 3) | (1 << 7)

static inline void set_mstatus(uint64 *x)
{
    *x |= MIE_MASK;
    *x &= ~MPP_MMASK;
    *x |= MPP_S; // set MPP to use Supervisor mode.
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

#define SATP_SV39 (8ULL << 60) // set 0x8 to satp's 60-63 bits

static inline void w_satp(uint64 x)
{
    asm volatile("csrw satp, %0"
                 :
                 : "r"(x)); // clear satp(pgdir address register) here.
}

static inline void tlb_flush()
{
    asm volatile("sfence.vma zero, zero");
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

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software
static inline uint64 r_sie()
{
    uint64 x;
    asm volatile("csrr %0, sie"
                 : "=r"(x));
    return x;
}

static inline void w_sie(uint64 x)
{
    asm volatile("csrw sie, %0"
                 :
                 : "r"(x));
}

// Machine-mode Interrupt Enable
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7)  // timer
#define MIE_MSIE (1L << 3)  // software
static inline uint64 r_mie()
{
    uint64 x;
    asm volatile("csrr %0, mie"
                 : "=r"(x));
    return x;
}

static inline void w_mie(uint64 x)
{
    asm volatile("csrw mie, %0"
                 :
                 : "r"(x));
}
