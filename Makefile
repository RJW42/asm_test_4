AS := nasm # Deal with it
ASFLAGS := -f elf32 # ELF binary in multiboot format ... mmm ... yummy

LD := ld
LDFLAGS := -melf_i386
LDFILE := link.ld

MKRESCUE := grub-mkrescue # On your system it might be called differently

all: iso

kernel: kernel-entry.o kernel.o
	$(LD) $(LDFLAGS) -T $(LDFILE) $^ -o $@

kernel.o: kernel.c
	gcc -m32 -ffreestanding -c $< -o $@

kernel-entry.o: kernel-entry.S
	$(AS) $(ASFLAGS) $< -o $@

iso: kernel
	mkdir -p isodir/boot/grub
	cp kernel isodir/boot/
	cp grub.cfg isodir/boot/grub
	$(MKRESCUE) -o hello-kernel.iso isodir

clean:
	rm -f *.o *.iso kernel isodir/boot/kernel 
