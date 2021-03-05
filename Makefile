COBJECTS = \
	main.o \
	start.o

SOBJECTS = \
	boot.o

QEMU = qemu-system-riscv64
TOOLCHAIN = riscv64-unknown-elf-
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)as
CFLAGS = -Wall -O2 -ffreestanding -nostdlib -mcmodel=medany

all: $(COBJECTS) $(SOBJECTS)

$(COBJECTS): %.o : %.c # use implicit rule

$(SOBJECTS): %.o : %.s

kernel: all
	$(CC) -T linker.ld -o kernel $(CFLAGS) $(COBJECTS) $(SOBJECTS)

clean:
	rm -f *.o kernel

QEMUOPTIONS = -bios none -kernel kernel

qemu: kernel
	$(QEMU) $(QEMUOPTIONS)
