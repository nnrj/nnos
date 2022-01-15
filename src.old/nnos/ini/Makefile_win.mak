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
RM			= del

#默认动作（执行不带参数的make时，默认执行make img）
default :
	$(MAKE) img

#文件生成规则
#./app/apilib.lib: Makefile
#$(MAKE) ./app/apilib/apilib.lib
#$(COPY) ./app/apilib/apilib.lib ./app/apilib.lib

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

syshead.bin : syshead.nas Makefile
	$(NASK) syshead.nas syshead.bin syshead.lst

fontbase.bin : fontbase.frc Makefile
	$(MAKEFONT) fontbase.frc fontbase.bin

fontbase.obj : fontbase.bin Makefile
	$(BIN2OBJ) fontbase.bin fontbase.obj _fonbase

bootpack.bim : $(OBJ_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJ_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.bin : bootpack.bim Makefile
	$(BIM2NEX) bootpack.bim bootpack.bin 0
    
nnos.sys : syshead.bin bootpack.bin Makefile
	$(COPY) /B syshead.bin+bootpack.bin nnos.sys 

nnos.img : ipl.bin nnos.sys Makefile ./app/sushu.nex \
		./app/helwin04.nex ./app/lines.nex ./app/snake.nex ./app/timers.nex \
		./app/beep.nex ./app/color02.nex ./app/crack07.nex ./app/vim.nex \
		./app/font/HZK16.fnt ./app/font/nihongo.fnt ./app/teststd.nex ./app/anbox.nex \
		./app/lineball.nex ./app/alienwar.nex ./app/caltor.nex ./app/notepad.nex ./app/music.nex \
		./app/picture/picture.nex
	$(EDIMG)   imgin:../tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		copy from:nnos.sys to:@: \
		copy from:./app/font/HZK16.fnt to:@: \
		copy from:./app/vim.nex to:@: \
		copy from:./app/notepad.nex to:@: \
		copy from:./app/caltor.nex to:@: \
		copy from:./app/music.nex to:@: \
		copy from:./app/music/ltstar.mml to:@: \
		copy from:./app/music/fate.mml to:@: \
		copy from:./app/music/goodbye.mml to:@: \
		copy from:./ini/test.txt to:@: \
		copy from:./ini/test02.txt to:@: \
		copy from:./app/sushu.nex to:@: \
		copy from:./app/helwin04.nex to:@: \
		copy from:./app/snake.nex to:@: \
		copy from:./app/timers.nex to:@: \
		copy from:./app/beep.nex to:@: \
		copy from:./app/color02.nex to:@: \
		copy from:./app/crack07.nex to:@: \
		copy from:./app/teststd.nex to:@: \
		copy from:./app/anbox.nex to:@: \
		copy from:./app/lineball.nex to:@: \
		copy from:./app/alienwar.nex to:@: \
		copy from:./app/picture/cheng.jpg to:@: \
		copy from:./app/picture/picture.nex to:@: \
		imgout:nnos.img

#copy from:./app/lifegame.nex to:@: \
#copy from:./app/music/goodbye.mml to:@: \
#copy from:./app/anbox.nex to:@: \
#copy from:./app/helwin04.nex to:@: \
#copy from:./app/lines.nex to:@: \
#copy from:./app/picture/cheng.jpg to:@: \
#copy from:./app/picture/picture.nex to:@: \
#copy from:./app/music/happy.mml to:@: \
#copy from:./app/music/goodbye.mml to:@: \		
#copy from:./app/chk03.nex to:@: \		
#copy from:./app/iroha.nex to:@: \
#copy from:./app/font/nihongo.fnt to:@: \
#copy from:./app/font/nihongo.frc to:@: \
#copy from:./app/hello05.nex to:@: \
#copy from:./app/ball.nex to:@: \
#copy from:./lib/996ICU to:@: \

./app/hello.nex : ./app/hello0.nas Makefile
	$(NASK) ./app/hello0.nas ./app/hello0.nex ./app/hello0.lst

./app/hello02.nex : ./app/hello02.nas Makefile
	$(NASK) ./app/hello02.nas ./app/hello02.nex ./app/hello02.lst

./app/a.bim : ./app/a.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/a.bim map:./app/a.map ./app/a.obj ./app/apilib.lib

./app/a.nex : ./app/a.bim Makefile
	$(BIM2NEX) ./app/a.bim ./app/a.nex 0

./app/hello03.bim : ./app/hello03.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/hello03.bim map:./app/hello03.map ./app/hello03.obj ./app/apilib.lib

./app/hello03.nex : ./app/hello03.bim Makefile
	$(BIM2NEX) ./app/hello03.bim ./app/hello03.nex 0

./app/hello04.bim : ./app/hello04.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/hello04.bim stack:1k map:./app/hello04.map \
		./app/hello04.obj ./app/apilib.lib

./app/hello04.nex : ./app/hello04.bim Makefile
	$(BIM2NEX) ./app/hello04.bim ./app/hello04.nex 0

./app/hello05.bim : ./app/hello05.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/hello05.bim stack:1k map:./app/hello05.map ./app/hello05.obj

./app/hello05.nex : ./app/hello05.bim Makefile
	$(BIM2NEX) ./app/hello05.bim ./app/hello05.nex 0

#./app/bug02.bim : ./app/bug02.obj ./app/apilib.lib Makefile
#$(OBJ2BIM) @$(RULEFILE) out:./app/bug02.bim map:./app/bug02.map ./app/bug02.obj ./app/apilib.lib

#./app/bug02.nex : ./app/bug02.bim Makefile
#$(BIM2NEX) ./app/bug02.bim ./app/bug02.nex 0

#./app/crack04.nex : ./app/crack04.asm Makefile
#nasm ./app/crack04.asm -o ./app/crack04.nex	

#./app/crack04.obj : ./app/crack04.asm Makefile
#$(NASK) ./app/crack04.nas ./app/crack04.obj ./app/crack04.lst

#./app/crack07.nex : ./app/crack07.asm Makefile
#nasm ./app/crack07.asm -o ./app/crack07.nex	

#./app/crack07.obj : ./app/crack07.asm Makefile
#$(NASK) ./app/crack07.nas ./app/crack07.obj ./app/crack07.lst

./app/crack07.bim : ./app/crack07.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/crack07.bim stack:1k map:./app/crack07.map ./app/crack07.obj

./app/crack07.nex : ./app/crack07.bim Makefile
	$(BIM2NEX) ./app/crack07.bim ./app/crack07.nex 0k

./app/helwin.bim : ./app/helwin.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/helwin.bim stack:1k map:./app/helwin.map \
		./app/helwin.obj ./app/apilib.lib

./app/helwin.nex : ./app/helwin.bim Makefile
	$(BIM2NEX) ./app/helwin.bim ./app/helwin.nex 0

./app/helwin02.bim : ./app/helwin02.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/helwin02.bim stack:1k map:./app/helwin02.map \
		./app/helwin02.obj ./app/apilib.lib

./app/helwin02.nex : ./app/helwin02.bim Makefile
	$(BIM2NEX) ./app/helwin02.bim ./app/helwin02.nex 0

./app/helwin03.bim : ./app/helwin03.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/helwin03.bim stack:1k map:./app/helwin03.map \
		./app/helwin03.obj ./app/apilib.lib

./app/helwin03.nex : ./app/helwin03.bim Makefile
	$(BIM2NEX) ./app/helwin03.bim ./app/helwin03.nex 40k
	
./app/helwin04.bim : ./app/helwin04.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/helwin04.bim stack:8k map:./app/helwin04.map \
		./app/helwin04.obj ./app/apilib.lib

./app/helwin04.nex : ./app/helwin04.bim Makefile
	$(BIM2NEX) ./app/helwin04.bim ./app/helwin04.nex 40k

./app/points.bim : ./app/points.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/points.bim stack:1k map:./app/points.map \
		./app/points.obj ./app/apilib.lib

./app/points.nex : ./app/points.bim Makefile
	$(BIM2NEX) ./app/points.bim ./app/points.nex 47k

./app/points02.bim : ./app/points02.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/points02.bim stack:1k map:./app/points02.map \
		./app/points02.obj ./app/apilib.lib

./app/points02.nex : ./app/points02.bim Makefile
	$(BIM2NEX) ./app/points02.bim ./app/points02.nex 47k

./app/lines.bim : ./app/lines.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/lines.bim stack:1k map:./app/lines.map \
		./app/lines.obj ./app/apilib.lib

./app/lines.nex : ./app/lines.bim Makefile
	$(BIM2NEX) ./app/lines.bim ./app/lines.nex 48k
	
#./app/ball.bim : ./app/ball.obj ./app/apilib.lib ./app/alloca.obj Makefile
#$(OBJ2BIM) @$(RULEFILE) out:./app/ball.bim stack:1k map:./app/ball.map \
#./app/ball.obj ./app/apilib.lib ./app/alloca.obj

#./app/ball.nex : ./app/ball.bim Makefile
#$(BIM2NEX) ./app/ball.bim ./app/ball.nex 60k

./app/snake.bim : ./app/snake.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/snake.bim stack:1k map:./app/snake.map \
		./app/snake.obj ./app/apilib.lib

./app/snake.nex : ./app/snake.bim Makefile
	$(BIM2NEX) ./app/snake.bim ./app/snake.nex 48k

./app/timers.bim : ./app/timers.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/timers.bim stack:1k map:./app/timers.map \
		./app/timers.obj ./app/apilib.lib
		
./app/timers.nex : ./app/timers.bim Makefile
	$(BIM2NEX) ./app/timers.bim ./app/timers.nex 40k
	
./app/beep.bim : ./app/beep.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/beep.bim stack:1k map:./app/beep.map \
		./app/beep.obj ./app/apilib.lib
		
./app/beep.nex : ./app/beep.bim Makefile
	$(BIM2NEX) ./app/beep.bim ./app/beep.nex 40k
	
./app/beep02.bim : ./app/beep02.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/beep02.bim stack:1k map:./app/beep02.map \
		./app/beep02.obj ./app/apilib.lib
		
./app/beep02.nex : ./app/beep02.bim Makefile
	$(BIM2NEX) ./app/beep02.bim ./app/beep02.nex 40k
	
./app/color.bim : ./app/color.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/color.bim stack:1k map:./app/color.map \
		./app/color.obj ./app/apilib.lib
		
./app/color.nex : ./app/color.bim Makefile
	$(BIM2NEX) ./app/color.bim ./app/color.nex 56k
	
./app/color02.bim : ./app/color02.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/color02.bim stack:1k map:./app/color02.map \
		./app/color02.obj ./app/apilib.lib
		
./app/color02.nex : ./app/color02.bim Makefile
	$(BIM2NEX) ./app/color02.bim ./app/color02.nex 56k
	
./app/sushu.bim : ./app/sushu.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/sushu.bim stack:11k map:./app/sushu.map \
		./app/sushu.obj ./app/apilib.lib
		
./app/sushu.nex : ./app/sushu.bim Makefile
	$(BIM2NEX) ./app/sushu.bim ./app/sushu.nex 0
	
./app/sushu02.bim : ./app/sushu02.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/sushu02.bim stack:11k map:./app/sushu02.map \
		./app/sushu02.obj ./app/apilib.lib
		
./app/sushu02.nex : ./app/sushu02.bim Makefile
	$(BIM2NEX) ./app/sushu02.bim ./app/sushu02.nex 0
	
./app/cattest.bim : ./app/cattest.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/cattest.bim stack:1k map:./app/cattest.map \
		./app/cattest.obj ./app/apilib.lib
		
./app/cattest.nex : ./app/cattest.bim Makefile
	$(BIM2NEX) ./app/cattest.bim ./app/cattest.nex 0

./app/vim.bim : ./app/vim.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/vim.bim stack:1k map:./app/vim.map \
		./app/vim.obj ./app/apilib.lib
		
./app/vim.nex : ./app/vim.bim Makefile
	$(BIM2NEX) ./app/vim.bim ./app/vim.nex 0k
	
./app/chlang.bim : ./app/chlang.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/chlang.bim stack:1k map:./app/chlang.map \
		./app/chlang.obj ./app/apilib.lib
		
./app/chlang.nex : ./app/chlang.bim Makefile
	$(BIM2NEX) ./app/chlang.bim ./app/chlang.nex 0
	
./app/chklang.bim : ./app/chklang.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/chklang.bim stack:1k map:./app/chklang.map \
		./app/chklang.obj ./app/apilib.lib
		
./app/chklang.nex : ./app/chklang.bim Makefile
	$(BIM2NEX) ./app/chklang.bim ./app/chklang.nex 0
	
./app/chk03.bim : ./app/chk03.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/chk03.bim stack:11k map:./app/chk03.map \
		./app/chk03.obj ./app/apilib.lib
		
./app/chk03.nex : ./app/chk03.bim Makefile
	$(BIM2NEX) ./app/chk03.bim ./app/chk03.nex 56k
	
./app/iroha.bim : ./app/iroha.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/iroha.bim stack:1k map:./app/iroha.map \
		./app/iroha.obj ./app/apilib.lib
		
./app/iroha.nex : ./app/iroha.bim Makefile
	$(BIM2NEX) ./app/iroha.bim ./app/iroha.nex 0k
	
./app/piano.bim : ./app/piano.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/piano.bim stack:11k map:./app/piano.map \
		./app/piano.obj ./app/apilib.lib
		
./app/piano.nex : ./app/piano.bim Makefile
	$(BIM2NEX) ./app/piano.bim ./app/piano.nex 56k
	
	
./app/teststd.bim : ./app/teststd.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/teststd.bim stack:1k map:./app/teststd.map \
		./app/teststd.obj ./app/apilib.lib
		
./app/teststd.nex : ./app/teststd.bim Makefile
	$(BIM2NEX) ./app/teststd.bim ./app/teststd.nex 0k
	
./app/testd02.bim : ./app/testd02.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/testd02.bim stack:1k map:./app/testd02.map \
		./app/testd02.obj ./app/apilib.lib
		
./app/testd02.nex : ./app/testd02.bim Makefile
	$(BIM2NEX) ./app/testd02.bim ./app/testd02.nex 0k

./app/anbox.bim : ./app/anbox.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/anbox.bim stack:8k map:./app/anbox.map \
		./app/anbox.obj ./app/apilib.lib

./app/anbox.nex : ./app/anbox.bim Makefile
	$(BIM2NEX) ./app/anbox.bim ./app/anbox.nex 0k
	
./app/lineball.bim : ./app/lineball.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/lineball.bim stack:52k map:./app/lineball.map \
		./app/lineball.obj ./app/apilib.lib

./app/lineball.nex : ./app/lineball.bim Makefile
	$(BIM2NEX) ./app/lineball.bim ./app/lineball.nex 0k
	
./app/alienwar.bim : ./app/alienwar.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/alienwar.bim stack:90k map:./app/alienwar.map \
		./app/alienwar.obj ./app/apilib.lib

./app/alienwar.nex : ./app/alienwar.bim Makefile
	$(BIM2NEX) ./app/alienwar.bim ./app/alienwar.nex 0k
	
./app/caltor.bim : ./app/caltor.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/caltor.bim stack:4k map:./app/caltor.map \
		./app/caltor.obj ./app/apilib.lib

./app/caltor.nex : ./app/caltor.bim Makefile
	$(BIM2NEX) ./app/caltor.bim ./app/caltor.nex 0k
	
./app/notepad.bim : ./app/notepad.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/notepad.bim stack:1024k map:./app/notepad.map \
		./app/notepad.obj ./app/apilib.lib

./app/notepad.nex : ./app/notepad.bim Makefile
	$(BIM2NEX) ./app/notepad.bim ./app/notepad.nex 0k
	
./app/music.bim : ./app/music.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/music.bim stack:256k map:./app/music.map \
		./app/music.obj ./app/apilib.lib

./app/music.nex : ./app/music.bim Makefile
	$(BIM2NEX) ./app/music.bim ./app/music.nex 40k
	
./app/lifegame.bim : ./app/lifegame.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/lifegame.bim stack:256k map:./app/lifegame.map \
		./app/lifegame.obj ./app/apilib.lib

./app/lifegame.nex : ./app/lifegame.bim Makefile
	$(BIM2NEX) ./app/lifegame.bim ./app/lifegame.nex 40k

# ./app/picture/bmp.bim : ./app/picture/bmp.obj ./app/picture/apilib.lib Makefile
# $(OBJ2BIM) @$(RULEFILE) out:./app/bmp.bim map:./app/bmp.map ./app/picture/bmp.obj ./app/apilib.lib

# ./app/picture/picture.bim : ./app/picture/picture.obj ./app/picture/jpeg.obj ./app/picture/bmp.obj ./app/apilib.lib Makefile
# $(OBJ2BIM) @$(RULEFILE) out:./app/picture/picture.bim stack:4480k map:./app/picture/picture.map \
# ./app/picture/picture.obj ./app/apilib.lib
./app/picture/picture.bim : ./app/picture/picture.obj ./app/picture/bmp.obj ./app/picture/jpeg.obj ./app/apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:./app/picture/picture.bim map:./app/picture/picture.map stack:4480k \
		./app/picture/picture.obj ./app/picture/jpeg.obj ./app/picture/bmp.obj ./app/apilib.lib

./app/picture/picture.nex : ./app/picture/picture.bim Makefile
	$(BIM2NEX) ./app/picture/picture.bim ./app/picture/picture.nex 0k
	
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
	$(MAKE)  ipl.bin
	$(MAKE) syshead.sys

asm :
	nasm ipl.asm -o ipl.bin -l ipl.lst
	nasm syshead.asm -o syshead.sys -l nnos.lst

getnask : 
	$(MAKE) ipl.nas
	$(MAKE) syshead.nas 
	$(MAKE) osfun.nas
	echo syshead.nas ;[INSTRSET [INSTRSET|.\ini\getnas.exe
	echo osfun.nas ;[ [|.\ini\getnas.exe
	echo syshead.nas ALIGN ALIGNB|.\ini\getnas.exe
	
img :
	$(MAKE) nnos.img

run :
	$(MAKE) img
	$(COPY) nnos.img ..\tools\qemu\fdimage0.bin
	$(MAKE)	-C ..\tools\qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: nnos.img

clean :
ifneq ($(wildcard *.lst),)
	$(RM) *.lst
endif
ifneq ($(wildcard *.bin),)
	$(RM) *.bin
endif
ifneq ($(wildcard *.bim),)
	$(RM) *.bim
endif
ifneq ($(wildcard *.obj),)
	$(RM) *.obj
endif
ifneq ($(wildcard *.gas),)
	$(RM) *.gas
endif
ifneq ($(wildcard *.map),)
	$(RM) *.map
endif
ifneq ($(wildcard *.o),)
	$(RM) *.o
endif
ifneq ($(wildcard *.s),)
	$(RM) *.s
endif
ifneq ($(wildcard *.S),)
	$(RM) *.S
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