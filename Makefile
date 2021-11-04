C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)

OBJ = ${C_SOURCES:.c=.o}

CC = gcc
CCFLAGS = -m32 -ffreestanding

AS := nasm # Deal with it
ASFLAGS := -f elf32 # ELF binary in multiboot format ... mmm ... yummy

LD := ld
LDFLAGS := -melf_i386
LDFILE := link.ld

MKRESCUE := grub-mkrescue # On your system it might be called differently


# Build Process 
all: iso

kernel.bin: kernel-entry.o ${OBJ}
	$(LD) $(LDFLAGS) -T $(LDFILE) $^ -o $@

#kernel.o: kernel.c
#	gcc -m32 -ffreestanding -c $< -o $@

kernel-entry.o: kernel-entry.asm
	$(AS) $(ASFLAGS) $< -o $@

iso: kernel.bin
	mkdir -p .isodir/boot/grub
	cp kernel.bin .isodir/boot/
	cp grub.cfg .isodir/boot/grub
	$(MKRESCUE) -o myos.iso .isodir

# Generics 
%.o: %.c ${HEADERS}
	${CC} ${CCFLAGS} -c $< -o $@

clean:
	rm -f *.o *.iso *.bin .isodir/boot/kernel  kernel/*.o drivers/*.o
