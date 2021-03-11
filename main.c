#include "kvm.h"
#include "type.h"
#include "string.h"

void main(void)
{
    uartinit();
    kinit();
    kvminit();     // kernel page table
    kvminithart(); // turn hardware paging on
    printf("booting...\n");
    asm volatile("1: j 1b");
}
