GCCPARAMS = -nostartfiles -e kernalmain
objects = mbr.o boot.o main.o
inputs = main.c


main.o:
	gcc $(GCCPARAMS) -o  $@ $(inputs)

%.o: %.s
	as -o $@ $<
	
mykernel.bin: linker.ld $(objects) 
	ld $(objects) --oformat=binary -o $@ -T $<

run: mykernel.bin
	qemu-system-i386 -fda $< -boot a -monitor stdio #-S -s # 

clean: 
	rm -f ./*.o ./*.bin 