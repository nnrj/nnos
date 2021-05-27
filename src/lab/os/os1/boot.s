

   
	.set KERNEL_SIZE, 2			# number of 512 byte blocks
	.set KERNEL_ADDRESS, 0x8200		# load kernel to this address
	.section .text
	.code16
	
_restart:
	movw	%cs,	%ax
	xorw	%ax,	%ax			# setup stack and segments
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%ss


	movb    $0x00,  %bh
	movb	$0x00,	%dh
	movb	$0x00,	%dl
	call	set_cursor2
	call	clear_screen2 

	movw    $0x07d0,  %cx  #size of char 80*25  0x50*0x19 
	movb    $0x00,  %bh      
	movb    $0x20,  %al  
	movb    $0x3f,  %bl
        movb    $0x09,  %ah

        int     $0x10

	
	movw	$msg2,	%ax
	movw	%ax,	%bp
	movw	len2,	%cx
	movb	$0x0d,	%dh
	movb	$0x1c,	%dl
	movb	$0x01,	%al
	movb	$0x13,	%ah
	movb	$0x3f,	%bl
	movb	$0x00,	%bh
	int	$0x10


	movw	$msg1,	%ax	#
	movw	%ax,	%bp
	movw	len1,	%cx	#

	movb	$0x0c,	%dh
	movb	$0x21,	%dl
	movb	$0x01,	%al
	movb	$0x13,	%ah
	movb	$0x3f,	%bl
	movb	$0x00,	%bh
	int	$0x10

load:
	movw	$0x00,	%dx	#dh->head,dl->driver
	movw	$0x0820,	%ax
	movw	%ax,	%es
	xorw	%bx,	%bx	#es:bx->the data   0x0820:0000
	movb	$0,	%ch	#ch->cidaohao
	movb	$4,	%cl	#cl->sector 
	movb	$2,	%ah	#param
	movb	$8,	%al	#how many blocks to read
	int	$0x13
	jnc 	gotonext
	jmp	load
gotonext:
	movb	$0x86,	%ah
	movw	$0x001e,	%cx
	movw	$0x8480,	%dx
	int	$0x15
	call	a20_enable


_end1:
	jmp	preswitch

set_cursor2:
	movb    $0x02,  %ah
        int     $0x10
	ret

clear_screen2:               
    movb    $0x06,  %ah     #  
    movb    $0,     %al     # 
    movb    $0,     %ch     # 
    movb    $0,     %cl     #    
    movb    $24,    %dh     # 
    movb    $79,    %dl     # 
    movb    $0x07,  %bh     #  
    int     $0x10  
    ret 

a20_enable:
	call	a20_wait
	movb	$0xd1,	%al			# write command
	outb	%al,	$0x64
	call	a20_wait
	movb	$0xdf,	%al			# enable a20  movb $0xdd, %al # disable a20		
	outb	%al,	$0x60
	call	a20_wait
	ret

a20_wait:
	inb	$0x64,	%al
	testb	$2,	%al
	jnz	a20_wait
	ret


preswitch:
	cli
	lgdt	gdt_pointer
	movl	%cr0,	%eax
	orl	$1,	%eax
	movl	%eax,	%cr0
	movl	$init_prot, %eax
	ljmp	$0b1000,	$init_prot
	

.code32
.extern kernal
init_prot:
	movw	$0b10000,	%ax
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	movw	%ax,	%ss

	#jmp	kernal
	ljmp	$0b1000, $0x8200	# jump to C file
	

	#movl	$hw,	%edi  # print string in code32
	#call	puts
myend:
	jmp	myend



puts:
	call	clear
	xorl	%ecx,	%ecx
putschar:
	movb	(%edi,%ecx),	%dl
	test	%dl,	%dl
	jz	putsend
	movb	$0x9f,	%dh
	movw	%dx,	(%ebx,%ecx,2)
	incl	%ecx
	jmp	putschar
putsend:
	ret

clear:
	xorl	%ecx,	%ecx
	movl	$0xb8000,	%ebx
clearchar:
	movb	$0x20,	%dl
	movb	$0x9f,	%dh	
	movl	$0x0f200f20,	(%ebx,%ecx,4)
	incl	%ecx
	cmpl	$1000,	%ecx
	jl	clearchar
	ret

 

gdt:
	.long 0,0

# code segment
	.word 0xffff		# limit (lower 16 bits)
	.word 0x0000		# base (lower 16 bits)
	.byte 0x00		# base (next 8 bits)
	.byte 0b10011010	# access byte

	.byte 0b11001111	# granularity byte
	.byte 0x00		# base (last 8 bits)

# data segment
	.word 0xffff		# limit (lower 16 bits)
	.word 0x0000		# base (lower 16 bits)
	.byte 0x00		# base (next 8 bits)
	.byte 0b10010010	# access byte

	.byte 0b11001111	# granularity byte
	.byte 0x00		# base (last 8 bits)

gdt_end:

gdt_pointer:
	.word gdt_end - gdt - 1	# size of GDT-1
	.long gdt		# GDT offset

hw:
	.string "Hello, World!"

msg1:
	.asciz "I'm bootloader!"
len1: 
	.int . - msg1

msg2:
	.asciz "My job is loading our OS!"
len2: 
	.int . - msg2


	.fill 0x1fe - (. - _restart) ,1,0 
	.word 0xaa55
