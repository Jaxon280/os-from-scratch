#include "int.h"
#include "string.h"
#include "uart.h"

#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)

#define PGSIZE 4096
#define PGROUNDUP(sz) (((uint64)(sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(sz) ((sz) & ~(PGSIZE - 1))

#define MAXVA (1L << 38)

#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 128 * 1024 * 1024)
#define CLINT 0x02000000L
#define PLIC 0x0C000000L
#define VIRTIO 0x10001000L

#define TRAMPOLINE (MAXVA - PGSIZE)
#define KSTACK(p) (TRAMPOLINE - ((p) + 1) * 2 * PGSIZE) // parameter p is n-th of process. includes guard page.

#define nullptr 0

#define MAXPROCS 64

typedef unsigned long pte_t;
typedef unsigned long *pagetable_t;

extern char etext[];
extern char end[];

pagetable_t kernel_pgtbl;

struct list
{
    struct list *next;
}; // linked list

struct list *kfreelist;

void *kalloc()
{
    struct list *alloc_list = kfreelist;

    if (alloc_list == nullptr)
        return nullptr;

    kfreelist = alloc_list->next;
    // todo: fill with junk
    return (void *)alloc_list;
}

void kfree(char *addr)
{
    struct list *l;
    if (((uint64)addr % PGSIZE != 0) || addr < end || (uint64)addr >= PHYSTOP)
    {
        printf("kfree: invalid memory address.");
        return;
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

#define OFFSET 12
#define VA_VPN(va, level) ((va >> ((level * 9) + OFFSET)) & 0x1ff)
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)
#define PTE2PPN(pte) (pte >> 10)

pte_t *walk(pagetable_t pgtbl, uint64 va)
{
    pte_t *pte;
    for (int level = 2; level > 0; level--)
    {
        pte = &pgtbl[VA_VPN(va, level)];
        if (*pte & PTE_V)
        {
            pgtbl = (pagetable_t)(PTE2PPN(*pte) * PGSIZE);
        }
        else
        {
            if ((pgtbl = (pte_t *)kalloc()) == nullptr)
                return nullptr;
            memset(pgtbl, 0, PGSIZE);
            *pte = PA2PTE(pgtbl) | PTE_V;
        }
    }

    return &pgtbl[VA_VPN(va, 0)];
}

void kvmmap(pagetable_t pgtbl, uint64 va, uint64 sz, uint64 pa, int perm)
{
    uint64 start;
    pte_t *pte;

    start = PGROUNDDOWN(va);
    pte = walk(pgtbl, start);
    if (*pte & PTE_V)
    {
        printf("invalid mapping");
        return;
    }
    *pte = PA2PTE(pa) | perm | PTE_V;
}

void mapstack(pagetable_t pgtbl)
{
    uint64 va;
    char *pa;
    for (int i = 0; i < MAXPROCS; i++)
    {
        pa = kalloc();
        if (pa == nullptr)
        {
            printf("kalloc failed.");
            return;
        }

        va = KSTACK(i);
        kvmmap(pgtbl, va, PGSIZE, (uint64)pa, PTE_R | PTE_W);
    }
}

void kvminit()
{
    //  kpgtbl is root pagetable address.
    pagetable_t kpgtbl = (pagetable_t)kalloc();

    memset(kpgtbl, 0, PGSIZE);

    kvmmap(kpgtbl, UART0, PGSIZE, UART0, PTE_R | PTE_W);
    kvmmap(kpgtbl, VIRTIO, PGSIZE, VIRTIO, PTE_R | PTE_W);
    kvmmap(kpgtbl, PLIC, 0x400000, PLIC, PTE_R | PTE_W);
    kvmmap(kpgtbl, KERNBASE, (uint64)etext - KERNBASE, KERNBASE, PTE_R | PTE_X);
    kvmmap(kpgtbl, (uint64)etext, PHYSTOP - (uint64)etext, (uint64)etext, PTE_R | PTE_W);

    mapstack(kpgtbl); // map kernel's stack for each processes
    kernel_pgtbl = kpgtbl;
}

void main(void)
{
    uartinit();
    kinit();
    kvminit();
    printf("booting...\n");
}
