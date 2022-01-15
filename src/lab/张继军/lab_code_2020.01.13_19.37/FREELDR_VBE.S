#
# zhangjijun
# 2021-10-21
#
.intel_syntax
.global     _start
#define STACK_SIZE 0x4000 

_start:
.code16
	xor 	ax,ax
	mov     ds,ax
	mov  	es,ax
	mov 	ss,ax
	mov 	sp,0x8000

# 卷屏方式清屏
clear_screen:
	mov     ah,0x06     # roll up all rows, clear the screen			
    mov     al,0x00     # row of left top corner
    mov     ch,0x00     # col of left top corner
    mov     cl,0x00     # row of right bottom corner
    mov     dh,0x18     # col of right bottom corner
    mov   	dl,0x4f     # property of roll up rows 
	mov     bh,0x07
	int     0x10			

# 设定vbe模式
set_video_mode:
	xor     ax ,ax
	mov     ds,ax
	mov     es,ax
	mov     di,0x800 	# VBE缓冲区的地址，最好在安全内存区域高于800H
# 检查是否支持VBE	
	mov     ax,0x4f00
	int     0x10
	cmp     ax,0x004f 
	jne		set_vga_0x13
	#movw	0x04(%di),	%ax
	mov     ax,[di+0x04]
	cmp		ax，0x200	# vbe版本 < 2.0
	jb		set_vga_0x13

# 检查vbe的0X118模式
	mov     cx,0x118
	mov     ax,0x4f01
	int     0x10
		
	cmp	    ah,0x00	   # 调用失败
	jne		set_vga_0x13

	cmp     al,0x4f    # 不支持VBE模式
	jne		set_vga_0x13
    mov     ax,[di]
	and     ax,0x0080  # 不支持 Linear Frame Buffer 内存模式
	jz		set_vga_0x13

# 保存LFB线性显存信息
	mov     [video_mode],0x118
	mov     ax,[di+0x12]
	mov     word ptr [screen_x],ax
	mov     ax,[di+0x14]
    mov     word ptr [screen_y],ax	
	mov     al,byte ptr [di+0x19]
	mov     byte ptr [bits_per_pixel],al
	mov     al,[di+0x1b]
	mov     byte ptr [memory_model],al
	mov     eax,dword ptr [di+0x28]
	mov     dword ptr [video_ram],eax	
	
# 设定vbe的0x118模式
	mov     bx,0x118
	add     bx,0x4000
	mov     ax,0x4f02
	int     0x10
	jmp     protected_mode
	
set_vga_0x13:
    mov     ah,0
	mov     al,0x13
	int     0x10

# 开始进入32位保护模式代码
protected_mode:
	cli
# 打开A20地址线使计算机寻址32位  	
    in      al,0x92
    or      al,0x2
    out     0x92,al
# cpu的PE位置1进入保护模式
	lgdt	[gdt_ptr]
	mov     eax,cr0
    or      eax,1
    mov     cr0,eax
# 清理CPU流水线及预取指令进入32位保护模式
	ljmp	0x0008,protcseg
# 32位保护模式代码	
.code32
protcseg:
    mov     ax,0x0010
    mov     ds,ax
    mov     es,ax
    mov     fs,ax
    mov     gs,ax
    mov     ss,ax  
# 设定堆栈的esp地址链接地址根据PBR规定是0x8000	
    mov     esp,offset _start+STACK_SIZE
# 开始读取C内核文件,完成loader使命	
    call    _bootmain   
1:
	jmp		1b
# 强制4字节对齐
.p2align    2
# GDT表设定
gdt:
	.quad	0x0000000000000000
	.quad	0x00cf9a000000ffff
	.quad	0x00cf92000000ffff
# 高速描述符表寄存器地址 	
gdt_ptr:
	.word	. - gdt - 1
	.long   gdt
# vbe模式需要的一些变量及类型
video_mode:
	.short  0
screen_x:
	.short	0 
screen_y:
	.short	0	
bits_per_pixel:
	.byte   0
memory_model:
	.byte   0
video_ram:
	.long	0

#.org	0x1fe,	0x90	# nop
#.word	0xaa55

