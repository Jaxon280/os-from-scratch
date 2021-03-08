#include "stdarg.h"

#include "int.h"
#include "uart.h"

#define NUMBER_CHAR(x) ((char)(x + 48))
#define HEX_CHAR(x) ((char)(x + 87))

void uartinit(void)
{
    // initialize uart.
    // WriteReg(IER, 0x00);

    // WriteReg(LCR, LCR_BAUD_LATCH);
    // WriteReg(0, 0x03);
    // WriteReg(1, 0x00);

    // WriteReg(LCR, LCR_EIGHT_BITS);
    // WriteReg(FCR, FIFO_ENABLE | FIFO_RESET);
    // WriteReg(IER, IER_R_ENABLE | IER_T_ENABLE);
    write_register(IER, 0x00);
    write_register(LCR, LCR_EIGHT_BITS);
    write_register(FCR, FIFO_ENABLE | FIFO_RESET);
    write_register(IER, IER_R_ENABLE | IER_T_ENABLE);
}

// polling version
void uartputc(char c)
{
    // while ((ReadReg(LSR) & LSR_T_EMPTY) == 0)
    //     ;
    // WriteReg(THR, c);
    while ((read_register(LSR) & LSR_T_EMPTY) == 0)
        ;
    write_register(THR, c);
}

char uartgetc(void)
{
    while ((read_register(LSR) & LSR_R_READY) == 0)
        ;
    char c = read_register(RHR);
    return c;
}

// void nprintf(char *v, int size)
// {
// }

void printint(int v, int base)
{
    int x = v;
    int sign = 0;
    char buf[16];

    if (x < 0)
    {
        sign = 1;
        x *= -1;
    }

    int d;
    int i = 0;

    do
    {
        d = (x % base);
        buf[i++] = ((d <= 9) ? NUMBER_CHAR(d) : HEX_CHAR(d));
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
    {
        uartputc(buf[i]);
    }
}

void printptr(uint64 v)
{
    uint64 x = (v & 0x00000000ffffffff);
    char buf[16];

    if (x < 0)
    {
        printf("invalid address\n"); // it will be changed to panic()
        return;
    }

    int d;
    int i = 0;

    do
    {
        d = (x % 16);
        buf[i++] = ((d <= 9) ? NUMBER_CHAR(d) : HEX_CHAR(d));
    } while ((x /= 16) != 0);

    uartputc('0');
    uartputc('x');
    while (--i >= 0)
    {
        uartputc(buf[i]);
    }
}

void printf(char *v, ...)
{
    va_list ap;
    va_start(ap, v);

    char c;

    for (int i = 0; (c = v[i]) != '\0'; i++)
    {
        if (c == '%')
        {
            c = v[++i];

            if (c == '\0')
            {
                uartputc('%');
                break;
            }

            switch (c)
            {
            case 'd':
                printint(va_arg(ap, int), 10);
                break;

            case 'c':
                c = 0xff & va_arg(ap, int);
                uartputc(c);
                break;

            case 'p':
                printptr(va_arg(ap, uint64));
                break;

            default:
                uartputc('%');
                uartputc(c);
                break;
            }
            continue;
        }

        uartputc(c);
    }

    va_end(ap);
}
