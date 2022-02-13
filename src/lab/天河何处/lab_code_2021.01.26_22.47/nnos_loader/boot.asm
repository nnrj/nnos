PROT_MODE_CSEG 	EQU 0x08		; 代码段选择子
PROT_MODE_DSEG 	EQU 0x10		; 数据段选择子
VBE_MODE 		EQU 0x105		; VBE模式定义

[BITS 16]
global start

start:
	CLI

set_video_mode:
	XOR AX, AX			; AX寄存器归零
	MOV DS, AX
	MOV ES, AX
	MOV DI, 0x800

; -----------------------------------------------------------
; 				功能0x00：返回VBE信息
; -----------------------------------------------------------
; 入口：
; 	AX			0x4F00
; 	ES：DI		指向VBE信息块的指针
; 出口：
; 	AX			VBE返回值
; ------------------------------------------------------------
; 			功能0x01：返回VBE特定模式信息
; ------------------------------------------------------------
; 入口：
; 	AX			0x4F01
; 	CX			模式号
; 	ES：DI		指向VBE特定模式信息块的指针
; 出口：
; 	AX			VBE返回值
; ------------------------------------------------------------
; 			功能0x02：设置VESA VBE 模式
; ------------------------------------------------------------
; 入口：
; 	AX			0x4F02
; 	BX			模式号
; 出口：
; 	AX			VBE返回值
; ------------------------------------------------------------
; 模式设置失败时，返回错误代码AH=01H
; VESA2.0以上增加了BX中D14，D15的位定义，完整定义如下：
; BX = 模式号
; 	D0～D8：9位模式号
; 	D9～D13：保留，必须为0
; 	D14 = 0：使用普通的窗口页面缓存模式，用VBE功能05H切换显示页面
; 		= 1：使用大的线性缓存区，其地址可从VBE功能01H的返回信息ModeInfo获得
; 	D15 = 0：清除显示内存
; 		= 1：不清除显示内存
; ------------------------------------------------------------
check_vbe:				; 调用显卡功能，检查是否支持VBE
	MOV AH, 0x4f		; 表明使用VBE标准
	MOV AL, 0x00		; 功能号0x00，用于返回VBE信息，返回值存入AX，若不支持VBE，AX的值会自动变回其他值
	INT 0x10			; 显卡的BIOS中断
	CMP AX, 0x004f		; 若有VBE，AX应该为0x004f
	JNE set_vga_mode	; 没有VBE则跳到字符模式
	MOV AX, [ES:DI+0x04]	; 若有VBE，判断VBE版本是否为2.0以上
	CMP AX, 0x0200		; 判断VBE版本是否为2.0以上
	JB set_vga_mode 	; 若VBE版本低于2.0，则跳到字符模式
check_vbe_mode:			; 检查VBE画面模式信息
	MOV CX, VBE_MODE	; 尝试设置画面模式
	MOV AH, 0x4f		; 表明使用VBE标准
	MOV AL, 0x01		; 功能号0x01，
	INT 0x10			; 调用0x10中断
	CMP 0x00			; 判断是否调用成功
	JNE	set_vga_mode	; 不成功，则跳到字符模式
	CMP 0x4f			; 成功，判断是否支持该模式
	JNE set_vga_mode	; 不支持该模式，跳到字符模式
	MOV AX, DI
	AND AX, 0x0080
	JZ	set_vga_mode	
save_video_mode_info:	; 设置VBE显卡参数
  MOV	video_mode, 0x105
  MOV  AX, [ES:DI+0x12]
  movw %ax,				   screen_x
  
  movw 0x14(%di),		   %ax
  movw %ax,				   screen_y
  movb 0x19(%di),		   %al
  movb %al,				   bits_per_pixel
  movb 0x1b(%di),		   %al
  movb %al,			       memory_model
  movl 0x28(%di),		   %eax
  movl %eax,			   video_ram
  movw $1,				   %ax
set_mode_vbe:							# 下面设置模式
  movw $0x105,	           %bx
  addw $0x4000,			   %bx			# BX第14个比特表示是否使用大的线性缓存区
  movb $0x4f,			   %ah			# 表示使用VBE标准
  movb $0x02,			   %al			# 功能号，表示设置模式
  int  $0x10
  jmp  pmode

set_mode_vga_0x13:						# 若不支持VBE则设置为VGA 0x13 mode
  movb $0,		           %ah          # 功能号0x0
  movb $0x13,	           %al          # 显示模式
  int  $0x10
  movw $320,			   screen_x
  movw $200,			   screen_y
  movl 0x28(%di),		   %eax
  movl %eax,			   video_ram
  ret
pmode:

  # Enable A20
  inb  $0x92, %al
  orb  $0x2, %al
  outb %al, $0x92

  # Load GDT
  lgdt gdtdesc
  
  # Switch from real to protected mode
  movl %cr0, %eax
  orl  $0x1, %eax
  movl %eax, %cr0

  # Jump into 32-bit protected mode
  ljmp $PROT_MODE_CSEG, $start32

.code32
start32:
  movw $PROT_MODE_DSEG, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movw %ax, %ss

  movl $start, %esp
  call _bootmain     # 从这里开始运行C代码
#用绿色清屏，测试LFB地址是否正确。  
/*  push %es
  push %ds
  pusha  
  movl %ds:(video_ram),%eax
  movl %eax,%ds:(lfbbase)
  movl %ds:(lfbbase),%edi
  movl $0x0,%edx
  movw %ds:(screen_x),%cx 
.lx:
  push %cx
  mov  %ds:(screen_y),%cx
.ly: 
  movb $0x3,(%edi,%edx,1)
  inc  %edx
  loop .ly
  pop  %cx
  loop .lx
  popa   
  pop  %ds
  pop  %es
  nop
  nop
*/  

spin:
  jmp  spin

.p2align 2
gdt:
  .quad	0x0000000000000000			# 空描述符
  .quad	0x00cf9a000000ffff			# 代码段描述符
  .quad	0x00cf92000000ffff			# 数据段描述符
  .quad	0x0000000000000000			# 留待以后使用
  .quad	0x0000000000000000			# 留待以后使用

gdtdesc:
  .word gdtdesc - gdt - 1
  .long gdt
video_mode:
  .short 0xff2
screen_x:							# 水平分辨率
  .short 0xff4 
screen_y:							# 垂直分辨率
  .short 0xff6	
bits_per_pixel:
  .byte  0
memory_model:
  .byte  0
video_ram:							# video_ram地址
  .long	 0xff8
 lfbbase:
  .long  0 	
	
