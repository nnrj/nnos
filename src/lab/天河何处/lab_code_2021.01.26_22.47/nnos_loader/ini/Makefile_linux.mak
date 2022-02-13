TOOLPATH = ../tools_linux/
LIBPATH = ../tools_linux/nnos/
SRCPATH = ./
APPPATH = ./app/

MAKE	    = make
NASM		= nasm
GCC			= gcc
OBJCOPY		= objcopy
OBJDUMP		= objdump
LD			= ld
QEMU		= qemu-system-i386
BOCHS		= bochs
BOCHSDBG	= bochsdbg
BXIMAGE		= bximage
ECHO		= echo
CD			= cd
DD			= dd
CP			= cp
RM			= rm
CAT			= cat

# 子编译目录
SUBDIRS = lib

# 递归编译
RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
        do \
        echo "making in $subdir"; \
        ( cd $subdir && $(MAKE) all -f Makefile -e CC='${CC}' CFLAG='${CFLAG}') || exit 1; \
        done
 
RECURSIVE_CLEAN= @for subdir in $(SUBDIRS); \
        do \
        echo "cleaning in $subdir"; \
        ( cd $subdir && $(MAKE) clean -f Makefile) || exit 1; \
        done

#默认动作（执行不带参数的make时，默认执行make img）
default :
	$(MAKE) img

# 子编译目录
SUBDIRS = lib

# 递归编译
RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
        do \
        echo "making in $subdir"; \
        ( cd $subdir && $(MAKE) all -f Makefile -e CC='${CC}' CFLAG='${CFLAG}') || exit 1; \
        done

RECURSIVE_CLEAN= @for subdir in $(SUBDIRS); \
        do \
        echo "cleaning in $subdir"; \
        ( cd $subdir && $(MAKE) clean -f Makefile) || exit 1; \
        done

# 默认动作（执行不带参数的make时，默认执行make img）
default :
	$(MAKE) img

# 文件生成规则
blank.img : 
ifneq ($(wildcard blank.img),)
	$(RM) blank.img
endif
	$(DD) if=/dev/zero of=blank.img bs=512 count=4194241

mbr.o : mbr.s
	$(GCC) -m32 -c -o mbr.o mbr.s

fat32.o : fat32.s
	$(GCC) -m32 -c -o fat32.o fat32.s

mbr.bin : mbr.o
	$(OBJCOPY) -O binary -j .text mbr.o mbr.bin

fat32.bin : fat32.o
	$(OBJCOPY) -O binary -j .text fat32.o fat32.bin

boot.o : boot.s
	$(GCC) -m32 -g -c boot.s

naskfunc.o : naskfunc.s
	$(GCC) -m32 -g -c naskfunc.s

bootmain.o : bootmain.c
	$(GCC) -m32 -g -fno-builtin -fno-pic -fno-stack-protector -nostdinc -Os -c bootmain.c

freeldr.elf : boot.o bootmain.o
	$(LD) -N -e start -Ttext=0x8000 -m elf_i386 -o freeldr.elf boot.o bootmain.o
	$(OBJDUMP) -S freeldr.elf > bootblock.asm

freeldr.sys : freeldr.elf
	$(OBJCOPY) -S -O binary -j .text freeldr.elf freeldr.sys

kernel.o : kernel.c
	$(GCC) -g -c -m32 -fno-pic -ffreestanding kernel.c -o kernel.o

kernel.elf : naskfunc.o kernel.o
	$(LD) -m elf_i386 -Tlinker.ld kernel.o naskfunc.o -o kernel.elf
	$(OBJDUMP) -SD $@ >$@.asm

sign : sign.c
	$(GCC) $< -o $@

kernel.img : mbr.bin fat32.bin freeldr.sys kernel.elf blank.img
ifneq ($(wildcard kernel.img),)
	$(DD) if=mbr.bin of=kernel.img bs=512 count=1 conv=notrunc
	$(DD) if=fat32.bin of=kernel.img bs=512 count=1 seek=63 conv=notrunc
	$(DD) if=fat32.bin of=kernel.img bs=512 skip=1 seek=66 count=1 conv=notrunc
	$(DD) if=freeldr.sys of=kernel.img seek=8279 conv=notrunc
	$(DD) if=kernel.elf of=kernel.img seek=8287 conv=notrunc
else
	$(CP) blank.img kernel.img
	$(DD) if=mbr.bin of=kernel.img bs=512 count=1 conv=notrunc
	$(DD) if=fat32.bin of=kernel.img bs=512 count=1 seek=63 conv=notrunc
	$(DD) if=fat32.bin of=kernel.img bs=512 skip=1 seek=66 count=1 conv=notrunc
	$(DD) if=freeldr.sys of=kernel.img seek=8279 conv=notrunc
	$(DD) if=kernel.elf of=kernel.img seek=8287 conv=notrunc
endif

# 通用文件生成规则
%.bin : %.asm
	$(NASM) $*.asm -o $*.bin

%.o : %.c
	$(GCC) $*.c -m32 -c  -ffreestanding -fno-builtin -o $*.o

# 命令
img :
	$(MAKE) kernel.img

lib :
	$(CD) lib && $(MAKE) lib -f Makefile -e GCC='${GCC}' && $(CD) ..

run :
	$(MAKE) run_b

run_b :
	$(MAKE) img
	$(ECHO) "[提示]使用Ctrl+C可退出bochs"
	$(BOCHS) -q -f bochsrc.bxrc

run_db :
	$(MAKE) img
	$(ECHO) "[提示]使用Ctrl+C可退出bochs"
	$(BOCHSDBG) -q -f bochsrc.bxrc

run_q :
	$(MAKE) img
	$(ECHO) "[提示]使用Ctrl+Alt+Q可退出qemu"
	$(CP) kernel.img ..\tools\qemu\fdimage0.bin
	$(MAKE)	-C ..\tools\qemu

#qemu-kernel : 
#	DISPLAY=:0 qemu-system-i386 -kernel kernel -monitor stdio

#qemu: kernel.img
#	DISPLAY=:0 qemu-system-i386 -drive file=$<,format=raw -monitor stdio

me :
	$(CP) ini\Makefile_win.mak Makefile

clean :
ifneq ($(wildcard *.lst),)
	$(RM) *.lst
endif
ifneq ($(wildcard *.bin),)
	$(RM) *.bin
endif
ifneq ($(wildcard *.elf),)
	$(RM) *.elf
endif
ifneq ($(wildcard *.obj),)
	$(RM) *.obj
endif
ifneq ($(wildcard *.o),)
	$(RM) *.o
endif
ifneq ($(wildcard *.map),)
	$(RM) *.map
endif
ifneq ($(wildcard *.sys),)
	$(RM) *.sys
endif

clean_log :
ifneq ($(wildcard ./ini/*.log),)
	$(RM) .\ini\*.log
endif

src_only :
	$(MAKE) clean
	$(MAKE) clean_log
ifneq ($(wildcard blank.img),)
	$(RM) blank.img
endif
ifneq ($(wildcard nnos.img),)
	$(RM) nnos.img
endif