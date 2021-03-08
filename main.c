#include "int.h"
#include "uart.h"
#define KERNBASE 0x80000000
#define PHYSTOP (KERNBASE + 128 * 1024 * 1024)

extern char end[];

char *a;

void kinit()
{
}

void main(void)
{
    uartinit();
    printf("%p", (uint64)end);
    // kinit();
    asm volatile("1: j 1b");
}
