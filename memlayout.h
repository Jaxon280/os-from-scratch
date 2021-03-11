#define MAXVA (1ULL << 38)
#define TRAMPOLINE (MAXVA - PGSIZE)
#define KSTACK(p) (TRAMPOLINE - ((p) + 1) * 2 * PGSIZE) // parameter p is n-th of process. includes guard page.
#define PHYSTOP (KERNBASE + 128 * 1024 * 1024)
#define KERNBASE 0x80000000L
#define VIRTIO 0x10001000L
#define UART0 0x10000000L // UART base address
#define PLIC 0x0C000000L
#define CLINT 0x02000000L
