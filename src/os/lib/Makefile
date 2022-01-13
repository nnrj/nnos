TOOLPATH = ../../tools/
LIBPATH = ../../tools/nnos/
SRCPATH = ./../
APPPATH = ./../app/
GCCPATH = ./../tools/gcc/

CFILES = clrerr.c fclose.c fdopen.c feof.c ferror.c fflush.c fgetc.c\
         fgetln.c fgetpos.c fgets.c fileno.c findfp.c flags.c\
         fopen.c fprintf.c fpurge.c fputc.c fputs.c fread.c\
         freopen.c fscanf.c fseek.c fsetpos.c ftell.c funopen.c\
         fvwrite.c fwalk.c fwrite.c getc.c getchar.c gets.c getw.c\
         makebuf.c mktemp.c perror.c printf.c putc.c putchar.c\
         puts.c putw.c refill.c remove.c rewind.c rget.c scanf.c\
         setbuf.c setbuffer.c setvbuf.c snprintf.c sprintf.c sscanf.c\
         stdio.c tempnam.c tmpfile.c tmpnam.c ungetc.c vfprintf.c\
         vfscanf.c vprintf.c vscanf.c vsnprintf.c vsprintf.c\
         vsscanf.c wbuf.c wsetup.c

MAKE	    = $(TOOLPATH)make/make.exe -r
NASM		= $(TOOLPATH)nasm/nasm.exe
GCC			= $(GCCPATH)/gcc.exe
QEMU		= $(TOOLPATH)qemu/qemu.exe
BOCHS		= $(TOOLPATH)bochs/bochs-win64.exe
BOCHSDBG	= $(TOOLPATH)bochs/bochsdbg-win64.exe
BXIMAGE		= $(TOOLPATH)bochs/bximage.exe
ECHO		= echo
DD			= $(TOOLPATH)dd/dd.exe
CP			= copy
RM			= del

# 默认动作（执行不带参数的make时，默认执行make img）
default :
	$(MAKE) lib

# 文件生成规则
blank.img : 
ifneq ($(wildcard blank.img),)
	$(RM) blank.img
endif
	$(DD) if=/dev/zero of=blank.img bs=60M count=1

# 通用文件生成规则
%.bin : %.asm
	$(NASM) $*.asm -o $*.bin
%.o : %.c
	$(GCC) $*.c -m32 -c  -ffreestanding -fno-builtin -o $*.o

# 命令
lib :
	gcc -o stdio.lib $(LIBFILES)

clean :
ifneq ($(wildcard *.lst),)
	$(RM) *.lst
endif
ifneq ($(wildcard *.bin),)
	$(RM) *.bin
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

src_only :
	$(MAKE) clean
ifneq ($(wildcard blank.img),)
	$(RM) blank.img
endif
ifneq ($(wildcard nnos.img),)
	$(RM) nnos.img
endif