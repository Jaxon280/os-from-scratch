.section .text # at 0x8000_0000
.global _boot

_boot:
    csrr t0, mhartid # Only single hardware thread is used here.
    bnez t0, _boot
    # Stack grows downside in RISC-V
    la sp, _stack
    li t1, 4096
    add sp, sp, t1
    call start
