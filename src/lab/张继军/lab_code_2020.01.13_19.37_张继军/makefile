#makefile for OS
#GCC_PATH = /mnt/zenglOX/opt/cross/bin
LD = ld
CC = cc
OB = objcopy
#CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
#CLINK_FLAGS = -ffreestanding -O2 -nostdlib
CFLAGS = -m32 -std=gnu99 -ffreestanding -gdwarf-2 -g3 -Wall -Wextra
CLINK_FLAGS = -ffreestanding -gdwarf-2 -g3 -nostdlib
ASFLAGS = -gdwarf-2 -g3

DEPS = common.h monitor.h 

OBJS = kernel.o main.o common.o monitor.o 
	
freeldr.elf: boot.o
	$(LD) -N -e start -Ttext=0x1000 -m elf_i386 -o freeldr.elf boot.o

kernel.elf: $(OBJS)
#	$(LD) -N -e start -Ttext=0x1000  -m elf_i386 -o freeldr.elf  $(OBJS)
	$(LD) -T linker.ld -Map fr.map -m elf_i386 -o kernel.elf  $(OBJS)

freeldr.bin:freeldr.elf
	$(OB) -S -O binary  -j .text -j .rdata  $< $@

kernel.bin:kernel.elf
	$(OB) -S -O binary -j .text -j .rdata $< $@

freeldr.sys:
#	copy /b freeldr.bin+kernel.bin freeldr.sys
	cat  freeldr.bin kernel.bin >freeldr.sys 
%.o: %.s
	$(CC) -m32 -c -o $@ $<

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

boot.o: boot.s
	$(CC) -m32 -c -o $@ $<

clean:
	rm -vf *.o
	rm -vf freeldr.sys
	rm -vf freeldr.elf

all: freeldr.bin kernel.bin freeldr.sys