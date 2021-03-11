#include "asm.h"
#include "alloc.h"
#include "type.h"
#include "string.h"
#include "memlayout.h"
#include "uart.h"

#define MAXPROCS 64

#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)

#define PGSIZE 4096
#define PGROUNDUP(sz) (((uint64)(sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(sz) ((sz) & ~(PGSIZE - 1))

#define OFFSET 12 // Both VA's and PA's offsets' size is 12bits
#define VA_VPN(va, level) ((va >> ((level * 9) + OFFSET)) & 0x1ff)
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)
#define PTE2PPN(pte) (pte >> 10)

#define nullptr 0

void *kalloc();
void kfree(char *addr);
void kinit();

pte_t *walk(pagetable_t pgtbl, uint64 va, int alloc);
void kvmmap(pagetable_t pgtbl, uint64 va, uint64 sz, uint64 pa, int perm);
void mapstack(pagetable_t pgtbl);
void kvminit();
void kvminithart();
