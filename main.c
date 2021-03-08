#include "int.h"

#define UART0 0x10000000 // UART base address
#define KERNBASE 0x80000000

#define BAUDRATE 0x10000

// UART registers' offset from UART0. These registers are memory-mapped.
#define RHR 0 // Receive Holding Register (read mode)
#define THR 0 // Transmit Holding Register (write mode)
#define IER 1 // Interrupt Enable Register
#define IER_R_ENABLE (1 << 0)
#define IER_T_ENABLE (1 << 1)
#define ISR 2 // Interrupt Status Register (Read Mode)
#define FCR 2 // FIFO Control Register (Write Mode)
#define FIFO_ENABLE (1 << 0)
#define FIFO_RESET (3 << 1)
#define LCR 3 // Line Control Register
#define LCR_EIGHT_BITS (3 << 0)
#define LCR_BAUD_LATCH (1 << 7) // enable to set baud rate
#define MCR 4                   // Modem Control Register
#define LSR 5                   // Line Status Register
#define LSR_R_READY (1 << 0)    // input is ready from RHR
#define LSR_T_EMPTY (1 << 5)    // output is ready from THR
#define MSR 6                   // Modem Status Register
#define SRR 7
#define SRW 7

#define Reg(reg) ((volatile unsigned char *)(UART0 + reg))

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// static inline void write_register(uint64 offset, char value)
// {
//     (*(volatile unsigned char *)(UART0 + offset)) = value;
// }

// static inline char read_register(uint64 offset)
// {
//     return (*(volatile unsigned char *)(UART0 + offset));
// }

void uartinit(void)
{
    // initialize uart.
    WriteReg(IER, 0x00);

    // WriteReg(LCR, LCR_BAUD_LATCH);
    // WriteReg(0, 0x03);
    // WriteReg(1, 0x00);

    WriteReg(LCR, LCR_EIGHT_BITS);
    WriteReg(FCR, FIFO_ENABLE | FIFO_RESET);
    WriteReg(IER, IER_R_ENABLE | IER_T_ENABLE);
    // write_register(IER, 0x00);
    // write_register(LCR, LCR_EIGHT_BITS);
    // write_register(FCR, FIFO_ENABLE | FIFO_RESET);
    // write_register(IER, IER_R_ENABLE | IER_T_ENABLE);
}

// polling version
void uartputc(char c)
{
    while ((ReadReg(LSR) & LSR_T_EMPTY) == 0)
        ;
    WriteReg(THR, c);
    // while ((read_register(LSR) & LSR_T_EMPTY) == 0)
    //     ;
    // write_register(THR, c);
}

// char uartgetc(void)
// {
//     while ((read_register(LSR) & LSR_R_READY) == 0)
//         ;
//     char c = read_register(RHR);
//     return c;
// }

// void nprintf(char *v, int size)
// {
// }

void printf(char *v)
{
    for (int i = 0; v[i] != 0; i++)
    {
        // int c = v[i];
        uartputc(v[i]);
    }
}

void main(void)
{
    uartinit();
    char *sample = "hello world";
    printf(sample);
    asm volatile("1: j 1b");
}
