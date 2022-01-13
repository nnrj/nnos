#makefile for zenglOS
#GCC_PATH = /mnt/zenglOX/opt/cross/bin
LD = ld
CC = cc
OB = objcopy
#CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
#CLINK_FLAGS = -ffreestanding -O2 -nostdlib
CFLAGS = -m32 -std=gnu99 -ffreestanding -gdwarf-2 -g3 -Wall -Wextra
CLINK_FLAGS = -ffreestanding -gdwarf-2 -g3 -nostdlib
ASFLAGS = -gdwarf-2 -g3

DEPS = common.h monitor.h descriptor_tables.h isr.h time.h kheap.h \
	paging.h

OBJS = boot.o kernel.o common.o monitor.o descriptor_tables.o \
	gdt.o interrupt.o isr.o time.o kheap.o paging.o

freeldr.elf: $(OBJS) linker.ld 
	$(LD) -N -e start -Ttext=0x8000 linker.ld -m elf_i386 -o freeldr.elf  $(OBJS)

freeldr.sys: freeldr.elf 
	$(OB) -S -O binary -j .text -j .rdata $@.elf $@.sys 

%.o: %.s
	$(CC) -m32 -c -o $@ $< 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

boot.o: bootI.S
	$(CC) -m32 -c -o $@ $<

clean:
	rm -vf *.o
	rm -vf freeldr.sys
	rm -vf freeldr.elf

all: freeldr.sys