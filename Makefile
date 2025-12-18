# -----------------------
#  Compiler / Assembler
# -----------------------
CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -m elf_i386

# -----------------------
#  Build Targets
# -----------------------
all: os-image.bin ataridisk.img

boot.bin: boot.asm
	$(AS) -f bin boot.asm -o boot.bin

kernel_entry.o: kernel_entry.asm
	$(AS) -f elf kernel_entry.asm -o kernel_entry.o

console.o: console.c console.h
	$(CC) $(CFLAGS) -c console.c -o console.o

idt.o: idt.c idt.h
	$(CC) $(CFLAGS) -c idt.c -o idt.o

idt_load.o: idt_load.asm
	$(AS) -f elf idt_load.asm -o idt_load.o

isr.o: isr.c isr.h
	$(CC) $(CFLAGS) -c isr.c -o isr.o

isr_asm.o: isr.asm
	$(AS) -f elf isr.asm -o isr_asm.o

irq.o: irq.c irq.h isr.h idt.h
	$(CC) $(CFLAGS) -c irq.c -o irq.o

irq_asm.o: irq.asm
	$(AS) -f elf irq.asm -o irq_asm.o

keyboard.o: keyboard.c keyboard.h input.h
	$(CC) $(CFLAGS) -c keyboard.c -o keyboard.o

shell.o: shell.c shell.h
	$(CC) $(CFLAGS) -c shell.c -o shell.o

string.o: string.c string.h
	$(CC) $(CFLAGS) -c string.c -o string.o

reboot.o: reboot.c reboot.h
	$(CC) $(CFLAGS) -c reboot.c -o reboot.o

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c -o input.o

calc.o: calc.c calc.h
	$(CC) $(CFLAGS) -c calc.c -o calc.o

fs.o: fs.c fs.h
	$(CC) $(CFLAGS) -c fs.c -o fs.o

port_io.o: port_io.c port_io.h
	$(CC) $(CFLAGS) -c port_io.c -o port_io.o

ata.o: ata.c ata.h
	$(CC) $(CFLAGS) -c ata.c -o ata.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.elf: kernel_entry.o kernel.o console.o idt.o idt_load.o \
	isr.o isr_asm.o irq.o irq_asm.o keyboard.o shell.o string.o reboot.o calc.o input.o fs.o port_io.o ata.o linker.ld
	$(LD) $(LDFLAGS) -T linker.ld \
		kernel_entry.o kernel.o console.o idt.o idt_load.o \
		isr.o isr_asm.o irq.o irq_asm.o keyboard.o shell.o string.o reboot.o calc.o input.o fs.o port_io.o ata.o \
		-o kernel.elf

kernel.bin: kernel.elf
	objcopy -O binary -j .text -j .rodata -j .data -j .bss kernel.elf kernel.bin

os-image.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > os-image.bin

# -----------------------
# Create 10MB Disk Image
# -----------------------
ataridisk.img:
	dd if=/dev/zero of=ataridisk.img bs=1M count=10

run: os-image.bin ataridisk.img
	qemu-system-i386 \
		-drive file=os-image.bin,format=raw,if=floppy \
		-drive file=ataridisk.img,format=raw,if=ide

clean:
	rm -f *.o *.bin *.elf os-image.bin ataridisk.img

.PHONY: all clean run
