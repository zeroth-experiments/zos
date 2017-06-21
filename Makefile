# make sure you install apt-get install grub-pc-bin
export SHELL = /bin/bash
export PREFIX="$(HOME)/opt/cross"
export TARGET=i686-elf
export PATH := $(PREFIX)/bin:$(PATH)
COMMANFBIN = $(PREFIX)/bin

CC=$(COMMANFBIN)/$(TARGET)-gcc
AS=$(COMMANFBIN)/$(TARGET)-as
CFLAGS=-ffreestanding -O2 -Wall -Wextra
LFLAGS=-ffreestanding -O2 -nostdlib
objects=boot.o kernel.o

#@echo selected AS as $(AS)

# i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
%.o: %.c
	$(CC) -c $< -o $@  -std=gnu99 $(CFLAGS)
# i686-elf-as boot.s -o boot.o
%.o: %.s
	$(AS) -o $@ $<


all: clean zeroth.iso

#i686-elf-gcc -T linker.ld -o mykernel.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
zeroth_kernel.bin: linker.ld $(objects)
	$(CC) -T $< -o $@ $(LFLAGS) $(objects) -lgcc

zeroth.iso: zeroth_kernel.bin
	mkdir -p iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "zerothos" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/zeroth_kernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso
	rm -rf iso
clean:
	rm *.o
	rm *.bin
	rm *.iso
