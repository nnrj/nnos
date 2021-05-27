OBJ_BOOTPACK = bootpack.obj osfun.obj fontbase.obj \
		gdtidt.obj interrupt.obj fifo.obj memery.obj graphics.obj devices.obj cover.obj timer.obj task.obj file.obj console.obj string.obj service.obj

TOOLPATH = ../tools/
LIBPATH = ../tools/nnos/
APPPATH = ./app/

MAKE	    = $(TOOLPATH)make.exe -r
NASK	    = $(TOOLPATH)nask.exe
EDIMG	    = $(TOOLPATH)edimg.exe
IMGTOL	    = $(TOOLPATH)imgtol.com
CC1         = $(TOOLPATH)cc1.exe -I$(LIBPATH) -Os -Wall -quiet
GAS2NASK    = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM     = $(TOOLPATH)obj2bim.exe  
BIM2NEX     = $(TOOLPATH)bim2nex.exe 
RULEFILE    = $(TOOLPATH)nnos/nnos.rul
IMGTOL      = $(TOOLPATH)imgtol.com
MAKEFONT	= $(TOOLPATH)makefont.exe
BIN2OBJ		= $(TOOLPATH)bin2obj.exe
COPY        = copy
DEL         = del

#默认动作（执行不带参数的make时，默认执行make img）
default :
	$(MAKE) img

#文件生成规则

kernelC.gas:kernelC.c
	$(CC1) kernelC.c -o kernelC.gas

kernelC.nas:kernelC.gas
	$(GAS2NASK) kernelC.gas kernelC.nas

k.bin:kernelC.gas kernelC.map
	obj2bim @nnos.rul out:k.bin stack:3136K map:kernelC.map kernelC.obj

loader.bin:cl.bin k.bin
	copy /b cl.bin+k.bin loader.bin

test.img:b.bin loader.bin
	dd if=b.bin of=test.img bs=512 count=1
	dd if=loader.bin of=test.img bs=512 count=4 seek=2

#通用文件生成规则
%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

%.nex : %.asm Makefile
	nasm $*.asm -o $*.nex
    
#命令
nas :
	$(MAKE)  loader.bin

img :
	$(MAKE) test.img

run :
	$(MAKE) img
	$(COPY) test.img ..\tools\qemu\fdimage0.bin
	$(MAKE)	-C ..\tools\qemu

run_bochs :
	$(MAKE) img
	.\bochsrc.bxrc

install :
	$(MAKE) img
	$(IMGTOL) w a: test.img

clean :
	$(DEL) *.lst
	$(DEL) *.bim
	$(DEL) *.obj
	$(DEL) *.gas	
	$(DEL) *.map	
	$(DEL) *.sys
	.\app\only.bat

src_only :
	$(Make) clean
	-del test.img