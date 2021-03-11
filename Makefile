COBJECTS = \
	start.o \
	main.o \
	uart.o \
	string.o \
	kvm.o \

SOBJECTS = \
	boot.o

QEMU = qemu-system-riscv64
TOOLCHAIN = riscv64-unknown-elf-
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)as
CFLAGS = -Wall -ffreestanding -O0 -nostdlib -mcmodel=medany -g3

all: $(SOBJECTS) $(COBJECTS)

$(SOBJECTS): %.o : %.s

$(COBJECTS): %.o : %.c # use implicit rule

kernel: all
	$(CC) -T linker.ld -o kernel $(CFLAGS) $(SOBJECTS) $(COBJECTS)

clean:
	rm -f *.o kernel

QEMUOPTIONS = -machine virt -bios none -kernel kernel -m 128M -smp 1
QEMUGRAPHIC = -monitor stdio
QEMUGDB = -s -S # By default, TCP port 1234 is used.

qemu-gdb: kernel
	$(QEMU) $(QEMUGDB) $(QEMUOPTIONS)

qemu: kernel
	$(QEMU) $(QEMUOPTIONS) $(QEMUGRAPHIC)
