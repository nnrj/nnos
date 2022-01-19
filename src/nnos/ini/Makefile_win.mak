SHELL=powershell
TOOLPATH = ../tools/
LIBPATH = ../tools/nnos/
GCCPATH = ../tools/MinGW64/bin/
SRCPATH = ./
APPPATH = ./app/

MAKE	    = $(TOOLPATH)make/make.exe -r
NASM		= $(TOOLPATH)nasm/nasm.exe
GCC			= $(GCCPATH)/gcc.exe
OBJCOPY		= $(GCCPATH)/objcopy.exe
OBJDUMP		= $(GCCPATH)/objdump.exe
LD			= $(GCCPATH)/ld.exe
QEMU		= $(TOOLPATH)qemu/qemu.exe
BOCHS		= $(TOOLPATH)bochs/bochs-win64.exe
BOCHSDBG	= $(TOOLPATH)bochs/bochsdbg-win64.exe
BXIMAGE		= $(TOOLPATH)bochs/bximage.exe
ECHO		= echo
CD			= cd
DD			= $(TOOLPATH)dd/dd.exe
CP			= copy
RM			= del

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
	$(DD) if=/dev/zero of=blank.img bs=60M count=1

mbr.bin : mbr.asm
	$(NASM) mbr.asm -I inc/ -o mbr.bin

loader.bin : loader.asm
	$(NASM) loader.asm -I inc/ -o loader.bin

nnos.sys : mbr.bin
	$(CP) /B mbr.bin nnos.sys

nnos.img : mbr.bin loader.bin nnos.sys blank.img
ifneq ($(wildcard nnos.img),)
	$(DD) if=mbr.bin of=nnos.img bs=512 count=1 conv=notrunc
	$(DD) if=loader.bin of=nnos.img bs=512 count=1 seek=2 conv=notrunc
else
	$(CP) blank.img nnos.img
	$(DD) if=mbr.bin of=nnos.img bs=512 count=1 conv=notrunc
	$(DD) if=loader.bin of=nnos.img bs=512 count=1 seek=2 conv=notrunc
endif

# 通用文件生成规则
%.bin : %.asm
	$(NASM) $*.asm -o $*.bin

%.o : %.c
	$(GCC) $*.c -m32 -c  -ffreestanding -fno-builtin -o $*.o

# 命令
img :
	$(MAKE) nnos.img

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
	$(QEMU) nnos.img

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