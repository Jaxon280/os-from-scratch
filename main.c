#include "int.h"
#include "uart.h"
#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 128 * 1024 * 1024)

#define PGSIZE 4096
#define PGROUNDUP(sz) (((uint64)(sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(sz) ((sz) & ~(PGSIZE - 1))

extern char end[];

struct list
{
    struct list *next;
}; // linked list

struct list *kfreelist;

void kfree(char *addr)
{
    struct list *l;
    if (((uint64)addr % PGSIZE != 0) || addr < end || (uint64)addr >= PHYSTOP)
    {
        printf("kfree: invalid memory address.");
    }

    // todo:  add filling with junk

    l = (struct list *)addr;
    l->next = kfreelist;
    kfreelist = l;
}

void kinit()
{
    char *cur_addr = (char *)PGROUNDUP(end); // cur_addr need to be aligned.
    char *end_addr = (char *)PHYSTOP;

    for (; cur_addr + PGSIZE < end_addr; cur_addr += PGSIZE)
    {
        kfree(cur_addr);
    }
}

void main(void)
{
    uartinit();
    kinit();
    printf("booting...\n");
    asm volatile("1: j 1b");
}
