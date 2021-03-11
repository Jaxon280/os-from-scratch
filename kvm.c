#include "kvm.h"

extern char etext[];
extern char end[];

pagetable_t kernel_pgtbl;

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

    memset(addr, 1, PGSIZE);
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

pte_t *walk(pagetable_t pgtbl, uint64 va, int alloc)
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
            if (!alloc || (pgtbl = (pte_t *)kalloc()) == nullptr)
                return nullptr;
            memset(pgtbl, 0, PGSIZE);
            *pte = PA2PTE(pgtbl) | PTE_V;
        }
    }

    return &pgtbl[VA_VPN(va, 0)];
}

void kvmmap(pagetable_t pgtbl, uint64 va, uint64 sz, uint64 pa, int perm)
{
    uint64 start, end;
    pte_t *pte;

    start = PGROUNDDOWN(va);
    end = PGROUNDUP(va + sz);
    for (;;)
    {
        pte = walk(pgtbl, start, 1);
        if (*pte & PTE_V)
        {
            printf("invalid mapping");
            return;
        }
        *pte = PA2PTE(pa) | perm | PTE_V;

        pa += PGSIZE;
        start += PGSIZE;

        if (start == end)
            break;
    }
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

void kvminithart()
{
    w_satp(SATP_SV39 | (((uint64)kernel_pgtbl) >> 12));
    tlb_flush();
}
