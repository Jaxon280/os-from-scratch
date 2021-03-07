COBJECTS = \
	main.o \
	start.o

SOBJECTS = \
	boot.o

QEMU = qemu-system-riscv64
TOOLCHAIN = riscv64-unknown-elf-
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)as
CFLAGS = -Wall -ffreestanding -O0 -nostdlib -mcmodel=medany -g3

all: $(COBJECTS) $(SOBJECTS)

$(COBJECTS): %.o : %.c # use implicit rule

$(SOBJECTS): %.o : %.s

kernel: all
	$(CC) -T linker.ld -o kernel $(CFLAGS) $(COBJECTS) $(SOBJECTS)

clean:
	rm -f *.o kernel

QEMUOPTIONS = -bios none -kernel kernel
QEMUGDB = -s -S # By default, TCP port 1234 is used.

qemu-gdb: kernel
	$(QEMU) $(QEMUGDB) $(QEMUOPTIONS)

qemu: kernel
	$(QEMU) $(QEMUOPTIONS)
