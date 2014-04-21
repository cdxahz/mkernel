CC		= gcc
ASM		= nasm
CFLAGS	= -m32 -c -fno-stack-protector
ASFLAGS	= -f elf32
LD		= ld
LDFILE	= -m elf_i386 -T link.ld -e 0x100010
LDFLAGS	= $(LDFILE)
TARGET	= kernel

all: boot.o kernel.o tty.o klib.o int.o interrupt.o keyboard.o list.o dsr.o mm.o
	$(LD) $(LDFLAGS) -o $(TARGET) boot.o kernel.o tty.o klib.o int.o keyboard.o interrupt.o list.o dsr.o mm.o

boot.o: kernel.asm
	$(ASM) $(ASFLAGS) kernel.asm -o boot.o

int.o: int.asm
	$(ASM) $(ASFLAGS) int.asm -o int.o

interrupt.o: int.c int.h
	$(CC) $(CFLAGS) int.c -o interrupt.o 

kernel.o: kernel.c
	$(CC) $(CFLAGS) kernel.c -o kernel.o

tty.o: tty.c tty.h
	$(CC) $(CFLAGS) tty.c -o tty.o

keyboard.o: keyboard.c keyboard.h
	$(CC) $(CFLAGS) keyboard.c -o keyboard.o

klib.o: klib.c klib.h tty.h
	$(CC) $(CFLAGS) klib.c -o klib.o

list.o: list.c list.h
	$(CC) $(CFLAGS) list.c -o list.o

dsr.o: dsr.c dsr.h
	$(CC) $(CFLAGS) dsr.c -o dsr.o

mm.o: mm.c mm.h multiboot.h
	$(CC) $(CFLAGS) -o mm.o mm.c

clean:
	-rm *.o
	-rm $(TARGET)
